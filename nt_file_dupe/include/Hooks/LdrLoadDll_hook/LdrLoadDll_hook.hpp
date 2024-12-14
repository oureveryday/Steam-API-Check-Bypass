#pragma once

namespace ntfsdupe::hooks {
    // https://doxygen.reactos.org/d7/d55/ldrapi_8c.html#a8838a6bd5ee2987045215ee7129c3a2c
    // https://github.com/wine-mirror/wine/blob/86557b9e0ba8a783f1b0d0918b1ddec7e0a7749e/dlls/ntdll/loader.c#L3372
    // Note: both docs seem incorrect, the first arg is the dll flags from LoadLibraryEx() | 1
    // ex: LOAD_LIBRARY_SEARCH_SYSTEM32 | 1
    // also on x64, first arg is sometimes used as ecx (4 bytes) and sometimes rcx (8 bytes)
    NTSTATUS NTAPI LdrLoadDll_hook(
        __in_opt  LPVOID            DllCharacteristics,
        __in_opt  LPDWORD           Unknown,
        __in      PUNICODE_STRING   DllName,
        _Out_     HMODULE*          BaseAddress
    );

    extern decltype(LdrLoadDll_hook) *LdrLoadDll_original;

}
