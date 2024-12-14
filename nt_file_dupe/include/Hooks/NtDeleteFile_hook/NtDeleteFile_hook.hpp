#pragma once

namespace ntfsdupe::hooks {
    NTSTATUS NTAPI NtDeleteFile_hook(
        __in POBJECT_ATTRIBUTES ObjectAttributes
    );

    extern decltype(NtDeleteFile_hook) *NtDeleteFile_original;
}
