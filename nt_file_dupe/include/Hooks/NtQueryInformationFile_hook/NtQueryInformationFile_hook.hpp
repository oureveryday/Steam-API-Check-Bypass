#pragma once

namespace ntfsdupe::hooks {
    NTSTATUS NTAPI NtQueryInformationFile_hook(
        __in  HANDLE                 FileHandle,
        __out PIO_STATUS_BLOCK       IoStatusBlock,
        __out PVOID                  FileInformation,
        __in  ULONG                  Length,
        __in  FILE_INFORMATION_CLASS FileInformationClass
    );

    extern decltype(NtQueryInformationFile_hook) *NtQueryInformationFile_original;

}
