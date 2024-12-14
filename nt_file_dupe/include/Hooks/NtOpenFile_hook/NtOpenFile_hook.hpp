#pragma once

namespace ntfsdupe::hooks {
    NTSTATUS NTAPI NtOpenFile_hook(
        __out PHANDLE            FileHandle,
        __in  ACCESS_MASK        DesiredAccess,
        __in  POBJECT_ATTRIBUTES ObjectAttributes,
        __out PIO_STATUS_BLOCK   IoStatusBlock,
        __in  ULONG              ShareAccess,
        __in  ULONG              OpenOptions
    );

    extern decltype(NtOpenFile_hook) *NtOpenFile_original;

}
