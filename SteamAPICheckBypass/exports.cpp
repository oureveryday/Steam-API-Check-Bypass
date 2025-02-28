#include "exports.h"
#include <stdio.h>
#include <windows.h>

FARPROC OriginalFuncs_version[17];
FARPROC OriginalFuncs_winmm[180];
FARPROC OriginalFuncs_winhttp[91];

enum DllType {
    DLL_VERSION,
    DLL_WINMM,
	DLL_WINHTTP,
    DLL_UNKNOWN
};

DllType GetCurrentDllType() {
    char modulePath[MAX_PATH] = {0};
    HMODULE hModule = NULL;
    
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCTSTR)GetCurrentDllType, &hModule);
    
    GetModuleFileNameA(hModule, modulePath, MAX_PATH);
    
    char* fileName = strrchr(modulePath, '\\');
    if (fileName) {
        fileName++;
        if (_stricmp(fileName, "version.dll") == 0) {
            return DLL_VERSION;
        } else if (_stricmp(fileName, "winmm.dll") == 0) {
            return DLL_WINMM;
        } else if (_stricmp(fileName, "winhttp.dll") == 0) {
            return DLL_WINMM;
        }
    }
    
    return DLL_UNKNOWN;
}

void Load()
{
    DllType currentDll = GetCurrentDllType();
    char szSystemDirectory[MAX_PATH] = {0};
    GetSystemDirectoryA(szSystemDirectory, MAX_PATH);
    
    if (currentDll == DLL_VERSION) {
        char OriginalPath[MAX_PATH] = {0};
        snprintf(OriginalPath, MAX_PATH, "%s\\version.dll", szSystemDirectory);
        
        HMODULE version = LoadLibraryA(OriginalPath);
        if (!version) {
            MessageBoxA(NULL, "Failed to load version.dll from system32\n", "Error", MB_ICONERROR);
            return;
        }
        
        for (int i = 0; i < 17; i++) {
            OriginalFuncs_version[i] = GetProcAddress(version, ExportNames_version[i]);
            if (!OriginalFuncs_version[i])
            {
                continue;
            }
        }
    } 
    else if (currentDll == DLL_WINMM) {
        char OriginalPath[MAX_PATH] = {0};
        snprintf(OriginalPath, MAX_PATH, "%s\\winmm.dll", szSystemDirectory);
        
        HMODULE winmm = LoadLibraryA(OriginalPath);
        if (!winmm) {
            MessageBoxA(NULL, "Failed to load winmm.dll from system32\n", "Error", MB_ICONERROR);
            return;
        }
        
        for (int i = 0; i < 180; i++) {
            OriginalFuncs_winmm[i] = GetProcAddress(winmm, ExportNames_winmm[i]);
            if (!OriginalFuncs_winmm[i]) {
                continue;
            }
        }
    }
    else if (currentDll == DLL_WINHTTP) {
        char OriginalPath[MAX_PATH] = { 0 };
        snprintf(OriginalPath, MAX_PATH, "%s\\winhttp.dll", szSystemDirectory);

        HMODULE winmm = LoadLibraryA(OriginalPath);
        if (!winmm) {
            MessageBoxA(NULL, "Failed to load winhttp.dll from system32\n", "Error", MB_ICONERROR);
            return;
        }

        for (int i = 0; i < 91; i++) {
            OriginalFuncs_winmm[i] = GetProcAddress(winmm, ExportNames_winmm[i]);
            if (!OriginalFuncs_winmm[i]) {
                continue;
            }
        }
    }
    else {
        return;
    }
}
