#pragma once

namespace ntfsdupe::hooks {
    NTSTATUS NTAPI NtOpenDirectoryObject_hook(
        __out PHANDLE            DirectoryHandle,
        __in  ACCESS_MASK        DesiredAccess,
        __in  POBJECT_ATTRIBUTES ObjectAttributes
    );

    extern decltype(NtOpenDirectoryObject_hook) *NtOpenDirectoryObject_original;

}
