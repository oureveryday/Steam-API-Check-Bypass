#include "Hooks/Hooks.hpp"


namespace ntfsdupe::hooks {
    decltype(NtQueryInformationFile_hook) *NtQueryInformationFile_original = nullptr;
}

// called from:
//   DeleteFileW
//   CreateFileInternal << CreateFileA | CreateFileW | CreateFile2
//   GetFileInformationByHandleEx
//   GetFileSize | GetFileSizeEx
//   RemoveDirectoryW
//   FindFirstFileNameW
//   CreateDirectoryExW
NTSTATUS NTAPI ntfsdupe::hooks::NtQueryInformationFile_hook(
    __in  HANDLE                 FileHandle,
    __out PIO_STATUS_BLOCK       IoStatusBlock,
    __out PVOID                  FileInformation,
    __in  ULONG                  Length,
    __in  FILE_INFORMATION_CLASS FileInformationClass
)
{
    try {
        auto result = NtQueryInformationFile_original(
            FileHandle,
            IoStatusBlock,
            FileInformation,
            Length,
            FileInformationClass
        );
        if (!NT_SUCCESS(result)) return result;
        
        wchar_t* name = nullptr;
        ULONG nameLength = 0;
        const ntfsdupe::cfgs::FileCfgEntry* cfg = nullptr;
        switch ((FILE_INFORMATION_CLASS_ACTUAL)FileInformationClass) {
        case FILE_INFORMATION_CLASS_ACTUAL::FileNameInformation:
        case FILE_INFORMATION_CLASS_ACTUAL::FileNormalizedNameInformation: {
            auto info = (PFILE_NAME_INFORMATION)FileInformation;
            name = (wchar_t*)&info->FileName;
            nameLength = info->FileNameLength;
            cfg = ntfsdupe::hooks::find_single_file_obj_ntpath(name, nameLength);
            break;
        }

        case FILE_INFORMATION_CLASS_ACTUAL::FileAllInformation:
        case FILE_INFORMATION_CLASS_ACTUAL::FileAlternateNameInformation: {
            auto info = (PFILE_ALL_INFORMATION)FileInformation;
            name = (wchar_t*)&info->NameInformation.FileName;
            nameLength = info->NameInformation.FileNameLength;
            cfg = ntfsdupe::hooks::find_single_file_obj_ntpath(name, nameLength);
            break;
        }

        // FileStreamInformation is used to get alternate files streams contained inside this file,
        // changing their names is not really useful, is it ?
        
        // https://learn.microsoft.com/en-us/windows/win32/fileio/hard-links-and-junctions#hard-links
        // FileHardLinkInformation returns the names of the links, not the original file
        // changing their names is not really useful, is it ?
        
        default: break;
        }

        // unsupported FileInformationClass
        if (!nameLength || !cfg) return result;

        // query APIs like this one has to be recrified because a process can detect it like this:
        // 1. call NtOpenFile() to get a file handle, which will redirect myfile.dll to myfile.rdr
        // 2. call NtQueryInformationFile() and specify filename as info type
        // in that case we have to change the name back from myfile.rdr to myfile.dll
        if (cfg->mode == ntfsdupe::cfgs::FileType::target) {
            NTFSDUPE_DBG(L"ntfsdupe::hooks::NtQueryInformationFile_hook target '%s'", cfg->target.c_str());
            const auto path_bytes = nameLength - cfg->filename_bytes;
            // write original file name
            memcpy(
                (char*)name + path_bytes,
                cfg->original_filename,
                cfg->filename_bytes
            );
        }

        return result;
    }
    catch (...) {
        NTFSDUPE_DBG(L"ntfsdupe::hooks::NtQueryInformationFile_hook exception occurred");
        return NtQueryInformationFile_original(
            FileHandle,
            IoStatusBlock,
            FileInformation,
            Length,
            FileInformationClass
        );
    }
}
