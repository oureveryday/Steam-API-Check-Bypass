#include "Hooks/Hooks.hpp"


namespace ntfsdupe::hooks {
    decltype(NtDeleteFile_hook) *NtDeleteFile_original = nullptr;
}


NTSTATUS NTAPI ntfsdupe::hooks::NtDeleteFile_hook(
    __in POBJECT_ATTRIBUTES ObjectAttributes
)
{
    try {
        auto cfg = find_single_file_obj_ntpath(ObjectAttributes);
        if (cfg) {
            switch (cfg->mode) {
            case ntfsdupe::cfgs::FileType::hide:
            case ntfsdupe::cfgs::FileType::target: { // target files are invisible to the process
                NTFSDUPE_DBG(
                    L"ntfsdupe::hooks::NtDeleteFile_hook hide/target '%s'",
                    std::wstring(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length / sizeof(wchar_t)).c_str()
                );
                // the original API doesn't change IoStatusBlock or FileHandle on failure
                // it returns STATUS_OBJECT_NAME_NOT_FOUND
                return STATUS_OBJECT_NAME_NOT_FOUND;
            }

            case ntfsdupe::cfgs::FileType::original: {
                cfg->hook_times.hook_times++;
                if (cfg->hook_times.mode == ntfsdupe::cfgs::HookTimesMode::nth_time_only && cfg->hook_times.hook_times != cfg->hook_times.hook_time_n) {
                    NTFSDUPE_DBG(L"ntfsdupe::hooks::NtDeleteFile_hook original '%s', %d times, return original", cfg->original.c_str(), cfg->hook_times.hook_times);
                    return NtDeleteFile_original(ObjectAttributes);
                }
                if (cfg->hook_times.mode == ntfsdupe::cfgs::HookTimesMode::not_nth_time_only && cfg->hook_times.hook_times == cfg->hook_times.hook_time_n) {
                    NTFSDUPE_DBG(L"ntfsdupe::hooks::NtDeleteFile_hook original '%s', %d times, return original", cfg->original.c_str(), cfg->hook_times.hook_times);
                    return NtDeleteFile_original(ObjectAttributes);
                }
                NTFSDUPE_DBG(L"ntfsdupe::hooks::NtDeleteFile_hook original '%s', %d times", cfg->original.c_str(), cfg->hook_times.hook_times);
                // it would be cheaper to just manipulate the original str, but not sure if that's safe

                auto len = (ObjectAttributes->ObjectName->Length - cfg->filename_bytes + (cfg->target.length() * sizeof(wchar_t)) ) + sizeof(wchar_t);;
                auto object_name_new = unique_ptr_stack(wchar_t, len);
                if (object_name_new) {
                    copy_new_file_target(object_name_new.get(), ObjectAttributes->ObjectName, cfg);

                    // backup original buffer
                    const auto buffer_backup = ObjectAttributes->ObjectName->Buffer;
                    const auto length_backup = ObjectAttributes->ObjectName->Length;
                    const auto max_length_backup = ObjectAttributes->ObjectName->MaximumLength;
                    ObjectAttributes->ObjectName->Length = static_cast<USHORT>(wcslen(ObjectAttributes->ObjectName->Buffer) * sizeof(wchar_t));
                    ObjectAttributes->ObjectName->MaximumLength = static_cast<USHORT>(len);
                    // set new buffer
                    ObjectAttributes->ObjectName->Buffer = object_name_new.get();
                    const auto result = NtDeleteFile_original(ObjectAttributes);
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

        return NtDeleteFile_original(ObjectAttributes);
    }
    catch (...) {
        NTFSDUPE_DBG(L"ntfsdupe::hooks::NtDeleteFile_hook exception occurred");
        return NtDeleteFile_original(ObjectAttributes);
    }
}