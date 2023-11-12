// Copyright © 2023 Xpl0itR
// 
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "pch.h"
#include "util.h"

#pragma region Forward functions to system version.dll
#pragma comment(linker, "/EXPORT:GetFileVersionInfoA=c:\\windows\\system32\\version.GetFileVersionInfoA")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoByHandle=c:\\windows\\system32\\version.GetFileVersionInfoByHandle")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExA=c:\\windows\\system32\\version.GetFileVersionInfoExA")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExW=c:\\windows\\system32\\version.GetFileVersionInfoExW")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeA=c:\\windows\\system32\\version.GetFileVersionInfoSizeA")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExA=c:\\windows\\system32\\version.GetFileVersionInfoSizeExA")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExW=c:\\windows\\system32\\version.GetFileVersionInfoSizeExW")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeW=c:\\windows\\system32\\version.GetFileVersionInfoSizeW")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoW=c:\\windows\\system32\\version.GetFileVersionInfoW")
#pragma comment(linker, "/EXPORT:VerFindFileA=c:\\windows\\system32\\version.VerFindFileA")
#pragma comment(linker, "/EXPORT:VerFindFileW=c:\\windows\\system32\\version.VerFindFileW")
#pragma comment(linker, "/EXPORT:VerInstallFileA=c:\\windows\\system32\\version.VerInstallFileA")
#pragma comment(linker, "/EXPORT:VerInstallFileW=c:\\windows\\system32\\version.VerInstallFileW")
#pragma comment(linker, "/EXPORT:VerLanguageNameA=c:\\windows\\system32\\version.VerLanguageNameA")
#pragma comment(linker, "/EXPORT:VerLanguageNameW=c:\\windows\\system32\\version.VerLanguageNameW")
#pragma comment(linker, "/EXPORT:VerQueryValueA=c:\\windows\\system32\\version.VerQueryValueA")
#pragma comment(linker, "/EXPORT:VerQueryValueW=c:\\windows\\system32\\version.VerQueryValueW")
#pragma endregion

BOOL APIENTRY DllMain(CONST HMODULE hModule, CONST DWORD fdwReason, CONST LPVOID lpvReserved)
{
    if (fdwReason != DLL_PROCESS_ATTACH)
        return TRUE;

    LPSTR fileStr = NULL;
    DWORD fileLen = 0;

    if (!OpenReadFileUtf8("libraries.txt", &fileStr, &fileLen))
    {
        MessageBoxA(NULL, "Failed to read libraries.txt", PROJECT_NAME, ErrBoxType);
        return TRUE;
    }

    LPSTR callerPath[MAX_PATH + 1];
    if (!GetModuleFileNameA(NULL, callerPath, MAX_PATH + 1))  // NOLINT(clang-diagnostic-incompatible-pointer-types)
    {
        MessageBoxA(NULL, "Failed to read caller path", PROJECT_NAME, ErrBoxType);
        return TRUE;
    }

    int read, hasTarget = FALSE, targetMatched = FALSE;
    for (LPSTR line = SkipBomUtf8(fileStr), fileEnd = fileStr + fileLen; line < fileEnd; line += read)
    {
        read = TerminateLine(line);

        if (*line == '#')
            continue;

        if (*line == '*')
        {
            if (!targetMatched)
            {
                hasTarget = TRUE;

                if (StrEndsWith(callerPath, line + 1))  // NOLINT(clang-diagnostic-incompatible-pointer-types)
                {
                    targetMatched = TRUE;
                }
            }

            continue;
        }

        if (hasTarget && !targetMatched)
            return TRUE;

        if (!LoadLibraryA(line))
        {
            MessageBoxA(NULL, line, PROJECT_NAME" - Failed to load library", ErrBoxType);
        }
    }

    HeapFree(GetProcessHeap(), 0, fileStr);
    return TRUE;
}