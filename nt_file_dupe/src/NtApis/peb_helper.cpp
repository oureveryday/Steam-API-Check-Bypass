#include "NtApis/peb_helper.hpp"
#include "NtApis/NtApis.hpp"

#include <winnt.h> // PEB stuff


// worth checking:
// https://learn.microsoft.com/en-us/archive/msdn-magazine/2002/march/windows-2000-loader-what-goes-on-inside-windows-2000-solving-the-mysteries-of-the-loader

// https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/pebteb/peb/index.htm
// https://www.wikiwand.com/en/Win32_Thread_Information_Block
// https://rvsec0n.wordpress.com/2019/09/13/routines-utilizing-tebs-and-pebs/
//#if defined _WIN64
////#define GetPeb() (PPEB)__readgsqword(FIELD_OFFSET(TEB, ProcessEnvironmentBlock))
//#define GetPeb() (PPEB)__readgsqword(0x60)
//#else // x32
////#define GetPeb() (PPEB)__readfsdword(FIELD_OFFSET(TEB, ProcessEnvironmentBlock))
//#define GetPeb() (PPEB)__readfsdword(0x30)
//#endif

// Visual Studio friendly way
#define GetPeb() NtCurrentTeb()->ProcessEnvironmentBlock




// https://github.com/wine-mirror/wine/blob/master/include/winternl.h
typedef struct
{
    const PVOID         _unused[2];
    LIST_ENTRY          InLoadOrderModuleList;
    LIST_ENTRY          InMemoryOrderModuleList;
    LIST_ENTRY          InInitializationOrderModuleList;
} *PPEB_LDR_DATA_MOD_LIST;

// https://github.com/wine-mirror/wine/blob/master/include/winternl.h
typedef struct
{
    LIST_ENTRY          InLoadOrderLinks;
    LIST_ENTRY          InMemoryOrderLinks;
    LIST_ENTRY          InInitializationOrderLinks;

    const PVOID         DllBase;
    const PVOID         EntryPoint;
    ULONG               SizeOfImage;
    UNICODE_STRING      FullDllName;
    UNICODE_STRING      BaseDllName;
    ULONG               Flags;
    SHORT               LoadCount;
    SHORT               TlsIndex;

    LIST_ENTRY          HashLinks;

    ULONG               TimeDateStamp;
    HANDLE              ActivationContext;
    const PVOID         Lock;
    const PVOID         DdagNode;

    LIST_ENTRY          NodeModuleLink;
} LDR_DATA_TABLE_ENTRY_MOD_LIST, * PLDR_DATA_TABLE_ENTRY_MOD_LIST;




static inline void UnlinkLdrEntry(PLDR_DATA_TABLE_ENTRY ldrEntry)
{
    PLIST_ENTRY ldrEntryHeader;

    // InMemoryOrderModuleList
    ldrEntryHeader = &((PLDR_DATA_TABLE_ENTRY_MOD_LIST)ldrEntry)->InMemoryOrderLinks;
    ldrEntryHeader->Blink->Flink = ldrEntryHeader->Flink;
    ldrEntryHeader->Flink->Blink = ldrEntryHeader->Blink;

    // InLoadOrderModuleList
    ldrEntryHeader = &((PLDR_DATA_TABLE_ENTRY_MOD_LIST)ldrEntry)->InLoadOrderLinks;
    ldrEntryHeader->Blink->Flink = ldrEntryHeader->Flink;
    ldrEntryHeader->Flink->Blink = ldrEntryHeader->Blink;

    // InInitializationOrderModuleList
    ldrEntryHeader = &((PLDR_DATA_TABLE_ENTRY_MOD_LIST)ldrEntry)->InInitializationOrderLinks;
    ldrEntryHeader->Blink->Flink = ldrEntryHeader->Flink;
    ldrEntryHeader->Flink->Blink = ldrEntryHeader->Blink;

    // HashLinks
    ldrEntryHeader = &((PLDR_DATA_TABLE_ENTRY_MOD_LIST)ldrEntry)->HashLinks;
    ldrEntryHeader->Blink->Flink = ldrEntryHeader->Flink;
    ldrEntryHeader->Flink->Blink = ldrEntryHeader->Blink;

    // NodeModuleLink
    ldrEntryHeader = &((PLDR_DATA_TABLE_ENTRY_MOD_LIST)ldrEntry)->NodeModuleLink;
    ldrEntryHeader->Blink->Flink = ldrEntryHeader->Flink;
    ldrEntryHeader->Flink->Blink = ldrEntryHeader->Blink;

}

static inline bool RemoveFromPeb_InMemoryOrder(PPEB peb, HMODULE hModule)
{
    PLIST_ENTRY loadedModulesHead = &peb->Ldr->InMemoryOrderModuleList;
    for (PLIST_ENTRY ldrEntryHeader = loadedModulesHead->Flink;
        ldrEntryHeader != loadedModulesHead;
        ldrEntryHeader = ldrEntryHeader->Flink)
    {
        PLDR_DATA_TABLE_ENTRY ldrEntry = CONTAINING_RECORD(ldrEntryHeader, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
        if (ldrEntry->DllBase == hModule)
        {
            UnlinkLdrEntry(ldrEntry);
            return true;
        }
    }

    return false;
}

static inline bool RemoveFromPeb_InLoadOrder(PPEB peb, HMODULE hModule)
{
    PLIST_ENTRY loadedModulesHead = &((PPEB_LDR_DATA_MOD_LIST)peb->Ldr)->InLoadOrderModuleList;
    for (PLIST_ENTRY ldrEntryHeader = loadedModulesHead->Flink;
        ldrEntryHeader != loadedModulesHead;
        ldrEntryHeader = ldrEntryHeader->Flink)
    {
        PLDR_DATA_TABLE_ENTRY ldrEntry =
            (PLDR_DATA_TABLE_ENTRY)CONTAINING_RECORD(ldrEntryHeader, LDR_DATA_TABLE_ENTRY_MOD_LIST, InLoadOrderLinks);
        if (ldrEntry->DllBase == hModule)
        {
            UnlinkLdrEntry(ldrEntry);
            return true;
        }
    }

    return false;
}

static inline bool RemoveFromPeb_InInitializationOrder(PPEB peb, HMODULE hModule)
{
    PLIST_ENTRY loadedModulesHead = &((PPEB_LDR_DATA_MOD_LIST)peb->Ldr)->InInitializationOrderModuleList;
    for (PLIST_ENTRY ldrEntryHeader = loadedModulesHead->Flink;
        ldrEntryHeader != loadedModulesHead;
        ldrEntryHeader = ldrEntryHeader->Flink)
    {
        PLDR_DATA_TABLE_ENTRY ldrEntry =
            (PLDR_DATA_TABLE_ENTRY)CONTAINING_RECORD(ldrEntryHeader, LDR_DATA_TABLE_ENTRY_MOD_LIST, InInitializationOrderLinks);
        if (ldrEntry->DllBase == hModule)
        {
            UnlinkLdrEntry(ldrEntry);
            return true;
        }
    }

    return false;
}




static inline HMODULE GetMyBaseUnsafe()
{
    PPEB peb = GetPeb();
    return (HMODULE)((void**)peb)[2]; // peb->ImageBaseAddress
}


// https://github.com/Apxaey/Unlinking-Modules-From-The-PEB-Dll-Hiding-
// https://github.com/SLAUC91/DLLHiding/blob/master/DLLHiding/Process.cpp
// https://www.youtube.com/watch?v=u2jFhdrHVg0

bool ntfsdupe::ntapis::peb_helper::remove_from_peb(HMODULE hModule)
{
    ntfsdupe::ntapis::LockPeb();

    PPEB peb = GetPeb();

    bool isCurrentProcess = false;
    if (!hModule) {
        hModule = GetMyBaseUnsafe();
        isCurrentProcess = true;
    } else if (hModule == GetMyBaseUnsafe()) {
        isCurrentProcess = true;
    }

    bool result =
        RemoveFromPeb_InMemoryOrder(peb, hModule) ||
        RemoveFromPeb_InLoadOrder(peb, hModule) ||
        RemoveFromPeb_InInitializationOrder(peb, hModule);

    if (isCurrentProcess) {
        ((void**)peb)[2] = NULL; // peb->ImageBaseAddress
    }

    ntfsdupe::ntapis::ReleasePeb();

    return result;
}
