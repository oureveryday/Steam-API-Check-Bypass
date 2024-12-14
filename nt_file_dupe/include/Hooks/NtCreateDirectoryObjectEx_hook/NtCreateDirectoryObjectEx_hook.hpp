#pragma once

namespace ntfsdupe::hooks
{
    // https://googleprojectzero.blogspot.com/2016/08/
    // https://docs.rs/ntapi/latest/ntapi/ntobapi/fn.NtCreateDirectoryObjectEx.html
    NTSTATUS NTAPI NtCreateDirectoryObjectEx_hook(
        __out     PHANDLE DirectoryHandle,
        __in      ACCESS_MASK DesiredAccess,
        __in      POBJECT_ATTRIBUTES ObjectAttributes,
        __in_opt  HANDLE ShadowDirectoryHandle,
        __in      ULONG Flags
    );

    extern decltype(NtCreateDirectoryObjectEx_hook) *NtCreateDirectoryObjectEx_original;

}
