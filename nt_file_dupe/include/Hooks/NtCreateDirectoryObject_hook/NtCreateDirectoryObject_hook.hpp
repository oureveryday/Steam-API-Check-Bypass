#pragma once

namespace ntfsdupe::hooks {
    NTSTATUS NTAPI NtCreateDirectoryObject_hook(
        __out PHANDLE            DirectoryHandle,
        __in  ACCESS_MASK        DesiredAccess,
        __in  POBJECT_ATTRIBUTES ObjectAttributes
    );

    extern decltype(NtCreateDirectoryObject_hook) *NtCreateDirectoryObject_original;
}

