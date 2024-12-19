#include "exports.h"
#include <stdio.h>
#include <windows.h>

FARPROC OriginalFuncs_version[17];

void Load()
{
    char szSystemDirectory[MAX_PATH] = {0};
    GetSystemDirectoryA(szSystemDirectory, MAX_PATH);

    char OriginalPath[MAX_PATH] = {0};
    snprintf(OriginalPath, MAX_PATH, "%s\\version.dll", szSystemDirectory);

    HMODULE version = LoadLibraryA(OriginalPath);
    // load version.dll from system32
    if (!version)
    {
        MessageBoxA(NULL, "Failed to load version.dll from system32\n", "Error", MB_ICONERROR);
        return;
    }

    // get addresses of original functions
    for (int i = 0; i < 17; i++)
    {
        OriginalFuncs_version[i] = GetProcAddress(version, ExportNames_version[i]);
        if (!OriginalFuncs_version[i])
        {
            MessageBoxA(NULL, "Failed to get address\n", "Error", MB_ICONERROR);
            return;
        }
    }
}
