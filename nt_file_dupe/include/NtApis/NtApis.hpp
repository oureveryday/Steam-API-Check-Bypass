#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winternl.h>

#ifndef NT_STATUS
    #define NT_STATUS(s) (s >= 0)
#endif

namespace ntfsdupe::ntapis {
    bool init(void);

    bool NtPathToDosPath(
        PWSTR dosPath, USHORT* dosPathBytes,
        PCWSTR ntPath, const USHORT ntPathBytes
    );

    bool GetFullDosPath(PWSTR fullPath, USHORT* fullPathBytes, PWSTR path);

    HANDLE DuplicateHandle(HANDLE original);

    NTSTATUS CloseHandle(HANDLE handle);

    void LockPeb();
    void ReleasePeb();
}
