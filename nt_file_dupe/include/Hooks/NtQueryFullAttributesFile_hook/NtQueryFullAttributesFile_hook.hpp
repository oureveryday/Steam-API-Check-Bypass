#pragma once

namespace ntfsdupe::hooks {
    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_network_open_information
    typedef struct _FILE_NETWORK_OPEN_INFORMATION {
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER AllocationSize;
        LARGE_INTEGER EndOfFile;
        ULONG         FileAttributes;
    } FILE_NETWORK_OPEN_INFORMATION, * PFILE_NETWORK_OPEN_INFORMATION;

    NTSTATUS NTAPI NtQueryFullAttributesFile_hook(
        __in  POBJECT_ATTRIBUTES             ObjectAttributes,
        __out PFILE_NETWORK_OPEN_INFORMATION FileInformation
    );

    extern decltype(NtQueryFullAttributesFile_hook) *NtQueryFullAttributesFile_original;
}
