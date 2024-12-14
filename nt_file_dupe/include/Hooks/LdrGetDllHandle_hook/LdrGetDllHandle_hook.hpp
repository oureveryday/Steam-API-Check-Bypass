#pragma once

namespace ntfsdupe::hooks {
    // https://doxygen.reactos.org/d7/d55/ldrapi_8c.html#a048f061547bbe72e1b884b368cf526a2
    // https://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FExecutable%20Images%2FLdrGetDllHandle.html
    NTSTATUS NTAPI LdrGetDllHandle_hook(
        _In_opt_ PWSTR 	      DllPath,
        _In_opt_ PULONG  	  DllCharacteristics,
        _In_ PUNICODE_STRING  DllName,
        _Out_ PHANDLE         DllHandle
    );

    extern decltype(LdrGetDllHandle_hook) *LdrGetDllHandle_original;
}
