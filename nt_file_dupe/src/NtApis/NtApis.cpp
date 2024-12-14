#include "NtApis/NtApis.hpp"
#include "Helpers/dbglog.hpp"

#include <malloc.h>
#include <unordered_set>
#include <iostream>

namespace ntfsdupe::ntapis {
    typedef struct _RTL_BUFFER {
        PUCHAR    Buffer;
        PUCHAR    StaticBuffer;
        SIZE_T    Size;
        SIZE_T    StaticSize;
        SIZE_T    ReservedForAllocatedSize; // for future doubling
        PVOID     ReservedForIMalloc; // for future pluggable growth
    } RTL_BUFFER, * PRTL_BUFFER;

    typedef struct _RTL_UNICODE_STRING_BUFFER {
        __in     UNICODE_STRING String;
        __in     RTL_BUFFER     ByteBuffer;
        __in_opt UCHAR          MinimumStaticBufferForTerminalNul[sizeof(WCHAR)];
    } RTL_UNICODE_STRING_BUFFER, * PRTL_UNICODE_STRING_BUFFER;

    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdmsec/nf-wdmsec-wdmlibrtlinitunicodestringex
    // https://github.com/wine-mirror/wine/blob/master/dlls/ntdll/rtlstr.c
    // https://github.com/mic101/windows/blob/master/WRK-v1.2/base/ntos/rtl/ntrtlp.h
    // https://github.com/tpn/winsdk-10/blob/master/Include/10.0.10240.0/shared/ntdef.h
    // https://github.com/mic101/windows/blob/6c4cf038dbb2969b1851511271e2c9d137f211a9/WRK-v1.2/public/sdk/inc/ntrtlpath.h
    typedef NTSTATUS(NTAPI* RtlNtPathNameToDosPathName_t)(
        __in        ULONG                      Flags,
        __inout     PRTL_UNICODE_STRING_BUFFER Path,
        __out_opt   PULONG                     Disposition,
        __inout_opt PWSTR*                     FilePart
    );

    static RtlNtPathNameToDosPathName_t RtlNtPathNameToDosPathName_original = nullptr;
}

namespace ntfsdupe::ntapis {
    typedef ULONG(NTAPI* RtlGetFullPathName_U_t)(
        _In_ PWSTR                             FileName,
        _In_ ULONG                             BufferLength,
        _Out_writes_bytes_(BufferLength) PWSTR Buffer,
        _Out_opt_ PWSTR*                       FilePart
    );

    static RtlGetFullPathName_U_t RtlGetFullPathName_U_original = nullptr;
}

namespace ntfsdupe::ntapis {
    typedef NTSTATUS (NTAPI* NtDuplicateObject_t)(
        __in            HANDLE      SourceProcessHandle,
        __in            HANDLE      SourceHandle,
        __in_opt        HANDLE      TargetProcessHandle,
        __out_opt       PHANDLE     TargetHandle,
        __in            ACCESS_MASK DesiredAccess,
        __in            ULONG       HandleAttributes,
        __in            ULONG       Options
    );

    static NtDuplicateObject_t NtDuplicateObject_original = nullptr;
}

namespace ntfsdupe::ntapis {
    typedef NTSTATUS(NTAPI* NtClose_t)(
        __in HANDLE Handle
    );

    static NtClose_t NtClose_original = nullptr;
}

namespace ntfsdupe::ntapis {
    // https://github.com/wine-mirror/wine/blob/master/dlls/ntdll/rtl.c
    typedef void (NTAPI* RtlAcquirePebLock_t)(void);

    typedef void (NTAPI* RtlReleasePebLock_t)(void);

    RtlAcquirePebLock_t RtlAcquirePebLock_original;
    RtlReleasePebLock_t RtlReleasePebLock_original;
}


bool ntfsdupe::ntapis::init(void)
{
    NTFSDUPE_DBG(L"ntfsdupe::ntapis::init()");
    auto NtdllHmod = GetModuleHandleW(L"ntdll.dll");
    if (!NtdllHmod) {
        NTFSDUPE_DBG(L"  failed to get ntdll.dll hmodule");
        return false;
    }

    RtlNtPathNameToDosPathName_original = (decltype(RtlNtPathNameToDosPathName_original))GetProcAddress(NtdllHmod, "RtlNtPathNameToDosPathName");
    if (!RtlNtPathNameToDosPathName_original) {
        NTFSDUPE_DBG(L"  failed to get addr of RtlNtPathNameToDosPathName");
        return false;
    }

    RtlGetFullPathName_U_original = (decltype(RtlGetFullPathName_U_original))GetProcAddress(NtdllHmod, "RtlGetFullPathName_U");
    if (!RtlGetFullPathName_U_original) {
        NTFSDUPE_DBG(L"  failed to get addr of RtlGetFullPathName_U");
        return false;
    }

    NtDuplicateObject_original = (decltype(NtDuplicateObject_original))GetProcAddress(NtdllHmod, (std::string("Nt") + "DuplicateObject").c_str());
    if (!NtDuplicateObject_original)
        NtDuplicateObject_original = (decltype(NtDuplicateObject_original))GetProcAddress(NtdllHmod, (std::string("Zw") + "DuplicateObject").c_str());
    if (!NtDuplicateObject_original) {
        NTFSDUPE_DBG(L"  failed to get addr of NtDuplicateObject");
        return false;
    }
    
    NtClose_original = (decltype(NtClose_original))GetProcAddress(NtdllHmod, (std::string("Nt") + "Close").c_str());
    if (!NtClose_original)
        NtClose_original = (decltype(NtClose_original))GetProcAddress(NtdllHmod, (std::string("Zw") + "Close").c_str());
    if (!NtClose_original) {
        NTFSDUPE_DBG(L"  failed to get addr of NtClose");
        return false;
    }
    
    RtlAcquirePebLock_original = (decltype(RtlAcquirePebLock_original))GetProcAddress(NtdllHmod, "RtlAcquirePebLock");
    if (!RtlAcquirePebLock_original) {
        NTFSDUPE_DBG(L"  failed to get addr of RtlAcquirePebLock");
        return false;
    }
    
    RtlReleasePebLock_original = (decltype(RtlReleasePebLock_original))GetProcAddress(NtdllHmod, "RtlReleasePebLock");
    if (!RtlReleasePebLock_original) {
        NTFSDUPE_DBG(L"  failed to get addr of RtlReleasePebLock");
        return false;
    }

    return true;
}


// https://googleprojectzero.blogspot.com/2016/02/the-definitive-guide-on-win32-to-nt.html
bool ntfsdupe::ntapis::NtPathToDosPath(
    PWSTR dosPath, USHORT* dosPathBytes,
    PCWSTR ntPath, const USHORT ntPathBytes)
{
    if (!dosPath || !dosPathBytes ||
        !ntPath || !*ntPath || !ntPathBytes)
        return false;
    
    USHORT required_bytes = ntPathBytes + sizeof(wchar_t);

    if (*dosPathBytes < required_bytes)
        return false;

    memcpy(dosPath, ntPath, ntPathBytes); // copy current nt path to dos path buffer
    dosPath[ntPathBytes / sizeof(wchar_t)] = L'\0';

    ntfsdupe::ntapis::RTL_UNICODE_STRING_BUFFER rtlStr;
    memset(&rtlStr, 0, sizeof(rtlStr));

    rtlStr.String.Buffer = (wchar_t*)dosPath;
    rtlStr.String.Length = ntPathBytes;
    rtlStr.String.MaximumLength = required_bytes;

    rtlStr.ByteBuffer.Buffer = (PUCHAR)dosPath;
    rtlStr.ByteBuffer.Size = ntPathBytes;

    ULONG conversion = 0;
    PWSTR unused = nullptr;
    NTSTATUS result = RtlNtPathNameToDosPathName_original(0, &rtlStr, &conversion, &unused);
    bool isOk = NT_SUCCESS(result) && (rtlStr.String.Length < *dosPathBytes);
    if (isOk) dosPath[rtlStr.String.Length / sizeof(wchar_t)] = L'\0';

    // adjust to the actual amount of bytes
    *dosPathBytes = rtlStr.String.Length;
    return isOk;
}


// https://googleprojectzero.blogspot.com/2016/02/the-definitive-guide-on-win32-to-nt.html
bool ntfsdupe::ntapis::GetFullDosPath(PWSTR fullPath, USHORT *fullPathBytes, PWSTR path)
{
    if (!path || !fullPathBytes) return false;

    // when this fails it returns the required amount of bytes including null terminator
    // when it succeedes it returns the amount of written bytes without null terminator
    PWSTR filepart = nullptr; // unused
    ULONG writtenBytes = RtlGetFullPathName_U_original(path, *fullPathBytes, fullPath, &filepart);

    //_freea(pathProper);

    bool isOk = writtenBytes < *fullPathBytes;
    *fullPathBytes = (USHORT)writtenBytes;
    if (isOk)
        fullPath[writtenBytes / sizeof(wchar_t)] = L'\0';

    return isOk;
}


// https://referencesource.microsoft.com/#mscorlib/microsoft/win32/win32native.cs,25aac1ead51b88ae
HANDLE ntfsdupe::ntapis::DuplicateHandle(HANDLE original)
{
#define DUPLICATE_SAME_ATTRIBUTES  0x00000004

    HANDLE result = INVALID_HANDLE_VALUE;
    NTSTATUS status = NtDuplicateObject_original(
        GetCurrentProcess(),
        original,
        GetCurrentProcess(),
        &result,
        0,
        0,
        DUPLICATE_SAME_ATTRIBUTES | DUPLICATE_SAME_ACCESS
    );
    if (!NT_SUCCESS(status)) return INVALID_HANDLE_VALUE;

    return result;

#undef DUPLICATE_SAME_ATTRIBUTES
}

NTSTATUS ntfsdupe::ntapis::CloseHandle(HANDLE handle)
{
    return NtClose_original(handle);
}

void ntfsdupe::ntapis::LockPeb(void)
{
    RtlAcquirePebLock_original();
}

void ntfsdupe::ntapis::ReleasePeb(void)
{
    RtlReleasePebLock_original();
}
