#pragma once

namespace ntfsdupe::hooks {
    NTSTATUS NTAPI NtQueryDirectoryFileEx_hook(
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
    );

    extern decltype(NtQueryDirectoryFileEx_hook) *NtQueryDirectoryFileEx_original;
}
