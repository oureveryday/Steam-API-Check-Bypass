#pragma once

namespace ntfsdupe::hooks {
    NTSTATUS NTAPI NtCreateFile_hook(
        __out          PHANDLE            FileHandle,
        __in           ACCESS_MASK        DesiredAccess,
        __in           POBJECT_ATTRIBUTES ObjectAttributes,
        __out          PIO_STATUS_BLOCK   IoStatusBlock,
        __in_opt       PLARGE_INTEGER     AllocationSize,
        __in           ULONG              FileAttributes,
        __in           ULONG              ShareAccess,
        __in           ULONG              CreateDisposition,
        __in           ULONG              CreateOptions,
        __in_opt       PVOID              EaBuffer,
        __in           ULONG              EaLength
    );

    extern decltype(NtCreateFile_hook) *NtCreateFile_original;
}

