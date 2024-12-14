#include "Hooks/Hooks.hpp"


namespace ntfsdupe::hooks {
    decltype(LdrGetDllHandle_hook) *LdrGetDllHandle_original = nullptr;
}


NTSTATUS NTAPI ntfsdupe::hooks::LdrGetDllHandle_hook(
    _In_opt_ PWSTR 	      DllPath,
    _In_opt_ PULONG  	  DllCharacteristics,
    _In_ PUNICODE_STRING  DllName,
    _Out_ PHANDLE         DllHandle
)
{
    try {
        if (DllName && DllName->Length && DllName->Buffer && DllName->Buffer[0]) {
            auto cfg = ntfsdupe::hooks::find_module_obj_filename(DllName);
            if (cfg) switch (cfg->mode)
            {
                // we only care about this case
            case ntfsdupe::cfgs::ModuleType::prevent_load: // this doesn't make sense here, but just in case
            case ntfsdupe::cfgs::ModuleType::target: // target modules are invisible to the process
            case ntfsdupe::cfgs::ModuleType::hide_handle: {
                NTFSDUPE_DBG(
                    L"ntfsdupe::hooks::LdrGetDllHandle_hook prevent_load/target/hide_handle '%s'",
                    std::wstring(DllName->Buffer, DllName->Length / sizeof(wchar_t)).c_str()
                );
                // this is what the original API returns when you pass a dll name that doesn't exist
                return STATUS_DLL_NOT_FOUND;
            }

            case ntfsdupe::cfgs::ModuleType::original: {
                NTFSDUPE_DBG(
                    L"ntfsdupe::hooks::LdrGetDllHandle_hook original '%s'",
                    std::wstring(DllName->Buffer, DllName->Length / sizeof(wchar_t)).c_str()
                );
                // it would be cheaper to just manipulate the original str, but not sure if that's safe
                auto len = wcslen(cfg->target_filename.c_str()) * sizeof(wchar_t) + sizeof(wchar_t);
                auto dllname_new = unique_ptr_stack(wchar_t, len);
                if (dllname_new) {
                    ntfsdupe::hooks::copy_new_module_target(dllname_new.get(), DllName, cfg);

                    USHORT fullDosPathBytes = 0;
                    ntfsdupe::ntapis::GetFullDosPath(NULL, &fullDosPathBytes, dllname_new.get());
                    if (fullDosPathBytes) {
                        auto fullDosPath = unique_ptr_stack(wchar_t, fullDosPathBytes);
                        if (fullDosPath) {
                            // convert dos path to full
                            bool conversion = ntfsdupe::ntapis::GetFullDosPath(fullDosPath.get(), &fullDosPathBytes, dllname_new.get());
                            if (conversion) {
                                ntfsdupe::helpers::upper(fullDosPath.get(), fullDosPathBytes / sizeof(wchar_t));
                                std::wstring_view wsv(fullDosPath.get(), fullDosPathBytes / sizeof(wchar_t));

                                ntfsdupe::cfgs::add_bypass(wsv);
                                // backup original buffer
                                const auto buffer_backup = DllName->Buffer;
                                const auto length_backup = DllName->Length;
                                const auto max_length_backup = DllName->MaximumLength;
                                // set new buffer
                                DllName->Buffer = dllname_new.get();
                                DllName->Length = static_cast<USHORT>(wcslen(DllName->Buffer) * sizeof(wchar_t));
                                DllName->MaximumLength = static_cast<USHORT>(len);
                                // call original API
                                const auto result = LdrGetDllHandle_original(
                                    DllPath,
                                    DllCharacteristics,
                                    DllName,
                                    DllHandle
                                );
                                // restore original buffer
                                DllName->Buffer = buffer_backup;
                                DllName->Length = length_backup;
                                DllName->MaximumLength = max_length_backup;
                                ntfsdupe::cfgs::remove_bypass(wsv);

                                return result;
                            }
                        }
                    }
                }
            }
            break;

            default: break;
            }

            // if redirection/hiding didn't do anything, then bypass this dll to avoid later redirection by NtOpenFile or NtCreateFile
            auto terminated_name = unique_ptr_stack(wchar_t, DllName->Length + sizeof(wchar_t));
            if (terminated_name) {
                memcpy(terminated_name.get(), DllName->Buffer, DllName->Length);
                terminated_name.get()[DllName->Length / sizeof(wchar_t)] = L'\0';

                USHORT fullDosPathBytes = 0;
                ntfsdupe::ntapis::GetFullDosPath(NULL, &fullDosPathBytes, terminated_name.get());
                if (fullDosPathBytes) {
                    auto fullDosPath = unique_ptr_stack(wchar_t, fullDosPathBytes);
                    if (fullDosPath) {
                        // convert dos path to full
                        bool conversion = ntfsdupe::ntapis::GetFullDosPath(fullDosPath.get(), &fullDosPathBytes, terminated_name.get());
                        if (conversion) {
                            ntfsdupe::helpers::upper(fullDosPath.get(), fullDosPathBytes / sizeof(wchar_t));
                            std::wstring_view wsv(fullDosPath.get(), fullDosPathBytes / sizeof(wchar_t));
                            ntfsdupe::cfgs::add_bypass(wsv);
                            const auto result = LdrGetDllHandle_original(
                                DllPath,
                                DllCharacteristics,
                                DllName,
                                DllHandle
                            );
                            ntfsdupe::cfgs::remove_bypass(wsv);
                            return result;
                        }
                    }
                }
            }
        }

        // if everything above failed
        return LdrGetDllHandle_original(
            DllPath,
            DllCharacteristics,
            DllName,
            DllHandle
        );
    }
    catch (...) {
        NTFSDUPE_DBG(L"ntfsdupe::hooks::LdrGetDllHandle_hook exception occurred");
        return LdrGetDllHandle_original(
            DllPath,
            DllCharacteristics,
            DllName,
            DllHandle
        );
    }
}
