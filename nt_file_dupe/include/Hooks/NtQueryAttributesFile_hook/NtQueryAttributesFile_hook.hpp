#pragma once

namespace ntfsdupe::hooks {
    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_basic_information
    typedef struct _FILE_BASIC_INFORMATION {
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        ULONG         FileAttributes;
    } FILE_BASIC_INFORMATION, * PFILE_BASIC_INFORMATION;

    NTSTATUS NTAPI NtQueryAttributesFile_hook(
        __in  POBJECT_ATTRIBUTES      ObjectAttributes,
        __out PFILE_BASIC_INFORMATION FileInformation
    );

    extern decltype(NtQueryAttributesFile_hook) *NtQueryAttributesFile_original;

}

