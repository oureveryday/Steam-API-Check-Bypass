#include "Hooks/Hooks.hpp"


namespace ntfsdupe::hooks {
    decltype(NtOpenFile_hook) *NtOpenFile_original = nullptr;
}


NTSTATUS NTAPI ntfsdupe::hooks::NtOpenFile_hook(
    __out PHANDLE            FileHandle,
    __in  ACCESS_MASK        DesiredAccess,
    __in  POBJECT_ATTRIBUTES ObjectAttributes,
    __out PIO_STATUS_BLOCK   IoStatusBlock,
    __in  ULONG              ShareAccess,
    __in  ULONG              OpenOptions
)
{
    try {
        auto cfg = find_single_file_obj_ntpath(ObjectAttributes);
        if (cfg) {
            switch (cfg->mode) {
            case ntfsdupe::cfgs::FileType::hide:
            case ntfsdupe::cfgs::FileType::target: { // target files are invisible to the process
                NTFSDUPE_DBG(
                    L"ntfsdupe::hooks::NtOpenFile_hook hide/target '%s'",
                    std::wstring(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length / sizeof(wchar_t)).c_str()
                );
                return STATUS_OBJECT_NAME_NOT_FOUND;
            }

            case ntfsdupe::cfgs::FileType::original: {
                if (ntfsdupe::cfgs::is_count_bypass(cfg)) {
                    return NtOpenFile_original(
                        FileHandle, DesiredAccess,
                        ObjectAttributes, IoStatusBlock,
                        ShareAccess, OpenOptions
                    );
                }
                NTFSDUPE_DBG(L"ntfsdupe::hooks::NtOpenFile_hook original '%s'", cfg->original.c_str());
                // it would be cheaper to just manipulate the original str, but not sure if that's safe

                auto len = (ObjectAttributes->ObjectName->Length - cfg->filename_bytes + (cfg->target.length() * sizeof(wchar_t)) ) + sizeof(wchar_t);;
                auto object_name_new = unique_ptr_stack(wchar_t, len);
                if (object_name_new) {
                    copy_new_file_target(object_name_new.get(), ObjectAttributes->ObjectName, cfg);

                    // backup original buffer
                    const auto buffer_backup = ObjectAttributes->ObjectName->Buffer;
                    const auto length_backup = ObjectAttributes->ObjectName->Length;
                    const auto max_length_backup = ObjectAttributes->ObjectName->MaximumLength;
                    // set new buffer
                    ObjectAttributes->ObjectName->Buffer = object_name_new.get();
                    ObjectAttributes->ObjectName->Length = static_cast<USHORT>(wcslen(ObjectAttributes->ObjectName->Buffer) * sizeof(wchar_t));
                    ObjectAttributes->ObjectName->MaximumLength = static_cast<USHORT>(len);
                    const auto result = NtOpenFile_original(
                        FileHandle, DesiredAccess,
                        ObjectAttributes, IoStatusBlock,
                        ShareAccess, OpenOptions
                    );
                    // restore original buffer
                    ObjectAttributes->ObjectName->Buffer = buffer_backup;
                    ObjectAttributes->ObjectName->Length = length_backup;
                    ObjectAttributes->ObjectName->MaximumLength = max_length_backup;
                    return result;
                }
            }
            break;

            default: break;
            }

        }

        return NtOpenFile_original(
            FileHandle, DesiredAccess,
            ObjectAttributes, IoStatusBlock,
            ShareAccess, OpenOptions
        );
    }
    catch (...) {
        NTFSDUPE_DBG(L"ntfsdupe::hooks::NtOpenFile_hook exception occurred");
        return NtOpenFile_original(
            FileHandle, DesiredAccess,
            ObjectAttributes, IoStatusBlock,
            ShareAccess, OpenOptions
        );
    }
}
