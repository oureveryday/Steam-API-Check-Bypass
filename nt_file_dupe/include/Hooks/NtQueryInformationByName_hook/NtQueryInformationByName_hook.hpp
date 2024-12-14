#pragma once

namespace ntfsdupe::hooks {
    NTSTATUS NTAPI NtQueryInformationByName_hook(
        __in  POBJECT_ATTRIBUTES     ObjectAttributes,
        __out PIO_STATUS_BLOCK       IoStatusBlock,
        __out PVOID                  FileInformation,
        __in  ULONG                  Length,
        __in  FILE_INFORMATION_CLASS FileInformationClass
    );

    extern decltype(NtQueryInformationByName_hook) *NtQueryInformationByName_original;
}
