#include "Hooks/Hooks.hpp"


namespace ntfsdupe::hooks {
    decltype(NtQueryDirectoryFileEx_hook)* NtQueryDirectoryFileEx_original = nullptr;
}

#if !defined(SL_RESTART_SCAN)
#define SL_RESTART_SCAN (0x00000001)
#endif

#if !defined(SL_RETURN_SINGLE_ENTRY)
#define SL_RETURN_SINGLE_ENTRY (0x00000002)
#endif

NTSTATUS NTAPI ntfsdupe::hooks::NtQueryDirectoryFileEx_hook(
    __in           HANDLE                 FileHandle,
    __in_opt       HANDLE                 Event,
    __in_opt       PIO_APC_ROUTINE        ApcRoutine,
    __in_opt       PVOID                  ApcContext,
    __out          PIO_STATUS_BLOCK       IoStatusBlock,
    __out          PVOID                  FileInformation,
    __in           ULONG                  Length,
    __in           FILE_INFORMATION_CLASS FileInformationClass,
    __in           ULONG                  QueryFlags,
    __in_opt       PUNICODE_STRING        FileName
)
{
    try {
        auto result = NtQueryDirectoryFileEx_original(
            FileHandle,
            Event,
            ApcRoutine,
            ApcContext,
            IoStatusBlock,
            FileInformation,
            Length,
            FileInformationClass,
            QueryFlags,
            FileName
        );
        if (!NT_SUCCESS(result)) return result;

        bool supported = false;
        ntfsdupe::hooks::MultiQueryOffsets_t query_info{};
        switch (FileInformationClass) {
        case FILE_INFORMATION_CLASS_ACTUAL::FileDirectoryInformation: {
            auto info = (PFILE_DIRECTORY_INFORMATION)FileInformation;
            supported = true;
            query_info.node_next_entry_offset = (unsigned long)((char*)&info->NextEntryOffset - (char*)info);
            query_info.node_filename_offset = (unsigned long)((char*)&info->FileName - (char*)info);
            query_info.node_filename_bytes_offset = (unsigned long)((char*)&info->FileNameLength - (char*)info);
        }
        break;

        case FILE_INFORMATION_CLASS_ACTUAL::FileFullDirectoryInformation: {
            auto info = (PFILE_FULL_DIR_INFORMATION)FileInformation;
            supported = true;
            query_info.node_next_entry_offset = (unsigned long)((char*)&info->NextEntryOffset - (char*)info);
            query_info.node_filename_offset = (unsigned long)((char*)&info->FileName - (char*)info);
            query_info.node_filename_bytes_offset = (unsigned long)((char*)&info->FileNameLength - (char*)info);
        }
        break;

        case FILE_INFORMATION_CLASS_ACTUAL::FileBothDirectoryInformation: {
            auto info = (PFILE_BOTH_DIR_INFORMATION)FileInformation;
            supported = true;
            query_info.node_next_entry_offset = (unsigned long)((char*)&info->NextEntryOffset - (char*)info);
            query_info.node_filename_offset = (unsigned long)((char*)&info->FileName - (char*)info);
            query_info.node_filename_bytes_offset = (unsigned long)((char*)&info->FileNameLength - (char*)info);
        }
        break;

        case FILE_INFORMATION_CLASS_ACTUAL::FileNamesInformation: {
            auto info = (PFILE_NAMES_INFORMATION)FileInformation;
            supported = true;
            query_info.node_next_entry_offset = (unsigned long)((char*)&info->NextEntryOffset - (char*)info);
            query_info.node_filename_offset = (unsigned long)((char*)&info->FileName - (char*)info);
            query_info.node_filename_bytes_offset = (unsigned long)((char*)&info->FileNameLength - (char*)info);
        }
        break;

        case FILE_INFORMATION_CLASS_ACTUAL::FileIdBothDirectoryInformation: {
            auto info = (PFILE_ID_BOTH_DIR_INFORMATION)FileInformation;
            supported = true;
            query_info.node_next_entry_offset = (unsigned long)((char*)&info->NextEntryOffset - (char*)info);
            query_info.node_filename_offset = (unsigned long)((char*)&info->FileName - (char*)info);
            query_info.node_filename_bytes_offset = (unsigned long)((char*)&info->FileNameLength - (char*)info);
        }
        break;

        case FILE_INFORMATION_CLASS_ACTUAL::FileIdFullDirectoryInformation: {
            auto info = (PFILE_ID_FULL_DIR_INFORMATION)FileInformation;
            supported = true;
            query_info.node_next_entry_offset = (unsigned long)((char*)&info->NextEntryOffset - (char*)info);
            query_info.node_filename_offset = (unsigned long)((char*)&info->FileName - (char*)info);
            query_info.node_filename_bytes_offset = (unsigned long)((char*)&info->FileNameLength - (char*)info);
        }
        break;

        case FILE_INFORMATION_CLASS_ACTUAL::FileIdGlobalTxDirectoryInformation: {
            auto info = (PFILE_ID_GLOBAL_TX_DIR_INFORMATION)FileInformation;
            supported = true;
            query_info.node_next_entry_offset = (unsigned long)((char*)&info->NextEntryOffset - (char*)info);
            query_info.node_filename_offset = (unsigned long)((char*)&info->FileName - (char*)info);
            query_info.node_filename_bytes_offset = (unsigned long)((char*)&info->FileNameLength - (char*)info);
        }
        break;

        case FILE_INFORMATION_CLASS_ACTUAL::FileIdExtdDirectoryInformation: {
            auto info = (PFILE_ID_EXTD_DIR_INFORMATION)FileInformation;
            supported = true;
            query_info.node_next_entry_offset = (unsigned long)((char*)&info->NextEntryOffset - (char*)info);
            query_info.node_filename_offset = (unsigned long)((char*)&info->FileName - (char*)info);
            query_info.node_filename_bytes_offset = (unsigned long)((char*)&info->FileNameLength - (char*)info);
        }
        break;

        case FILE_INFORMATION_CLASS_ACTUAL::FileIdExtdBothDirectoryInformation: {
            auto info = (PFILE_ID_EXTD_BOTH_DIR_INFORMATION)FileInformation;
            supported = true;
            query_info.node_next_entry_offset = (unsigned long)((char*)&info->NextEntryOffset - (char*)info);
            query_info.node_filename_offset = (unsigned long)((char*)&info->FileName - (char*)info);
            query_info.node_filename_bytes_offset = (unsigned long)((char*)&info->FileNameLength - (char*)info);
        }
        break;
        }

        if (!supported) return result;

        ULONG currentNodeBytes = *(ULONG*)((char*)FileInformation + query_info.node_next_entry_offset);

        // if the caller was querying a single object (via FindFirstFileExW() for example)
        if (!currentNodeBytes) {
            auto cfg = ntfsdupe::hooks::find_single_file_obj_base_handle(query_info, FileHandle, FileInformation);
            if (cfg) switch (cfg->mode) {
            case ntfsdupe::cfgs::FileType::target:
            case ntfsdupe::cfgs::FileType::hide: {
                NTFSDUPE_DBG(
                    L"ntfsdupe::hooks::NtQueryDirectoryFileEx_hook hide/target '%s'",
                    std::wstring(
                        (wchar_t*)((char*)FileInformation + query_info.node_filename_offset),
                        *(ULONG*)((char*)FileInformation + query_info.node_filename_bytes_offset) / sizeof(wchar_t)
                    ).c_str()
                );
                IoStatusBlock->Information = 0;
                return STATUS_OBJECT_NAME_NOT_FOUND;
            }
            break;

            case ntfsdupe::cfgs::FileType::original: { // then redirect to target
                NTFSDUPE_DBG(L"ntfsdupe::hooks::NtQueryDirectoryFileEx_hook original '%s'", cfg->original.c_str());
                // duplicate the handle
                auto handle_dup = ntfsdupe::ntapis::DuplicateHandle(FileHandle);
                if (handle_dup == INVALID_HANDLE_VALUE) return result;

                // target file info
                auto file_info_target = unique_ptr_stack(void, IoStatusBlock->Information);
                if (!file_info_target) return result;

                // create a copy of the target filename because we need a non const str
                auto len = sizeof(cfg->target_filename);
                auto target_filename_copy = unique_ptr_stack(wchar_t, len);
                if (!target_filename_copy) return result;

                memcpy(target_filename_copy.get(), cfg->target_filename, len);
                UNICODE_STRING ustr;
                ustr.Length = len;
                ustr.MaximumLength = len;
                ustr.Buffer = target_filename_copy.get();

                // query the target file instead of the original
                IO_STATUS_BLOCK target_Io_status;
                NTSTATUS queryStatus = NtQueryDirectoryFileEx_original(
                    handle_dup,
                    NULL,
                    NULL,
                    NULL,
                    &target_Io_status,//IoStatusBlock,
                    file_info_target.get(),//FileInformation,
                    (ULONG)IoStatusBlock->Information, //Length,
                    FileInformationClass,
                    SL_RESTART_SCAN | SL_RETURN_SINGLE_ENTRY,
                    &ustr
                );
                ntfsdupe::ntapis::CloseHandle(handle_dup);

                if (!NT_SUCCESS(queryStatus)) {
                    memcpy(IoStatusBlock, &target_Io_status, sizeof(target_Io_status));
                    return queryStatus;
                }

                memcpy(FileInformation, file_info_target.get(), target_Io_status.Information);
            }
                                                   break;

            default: break;
            }

            return result;
        }

        return ntfsdupe::hooks::handle_file_multi_query(query_info, FileHandle, FileInformation, IoStatusBlock, result);
    }
    catch (...) {
        NTFSDUPE_DBG(L"ntfsdupe::hooks::NtQueryDirectoryFileEx_hook exception occurred");
        return NtQueryDirectoryFileEx_original(
            FileHandle,
            Event,
            ApcRoutine,
            ApcContext,
            IoStatusBlock,
            FileInformation,
            Length,
            FileInformationClass,
            QueryFlags,
            FileName
        );
    }
}
