// Copyright © 2023 Xpl0itR
// 
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "pch.h"

static BOOL OpenFileRead(CONST LPCSTR lpFileName, CONST LPHANDLE fileHandle)
{
    OFSTRUCT ofStruct;
    HFILE hFile = OpenFile(lpFileName, &ofStruct, OF_READ | OF_PROMPT);

    *fileHandle = (HANDLE)(INT_PTR)hFile;  // NOLINT(performance-no-int-to-ptr)
    return hFile != HFILE_ERROR;
}

static BOOL ReadFileUtf8(CONST HANDLE fileHandle, LPSTR* fileString, CONST LPDWORD fileLength)
{
    DWORD fileLen = GetFileSize(fileHandle, NULL);
    *fileLength = fileLen;

    HANDLE heap = GetProcessHeap();
    LPSTR buffer = HeapAlloc(heap, 0, fileLen + 1);
    if (!buffer)
        return FALSE;

    if (!ReadFile(fileHandle, buffer, fileLen, NULL, NULL))
    {
        HeapFree(heap, 0, buffer);
        return FALSE;
    }

    buffer[fileLen] = '\0';
    *fileString = buffer;

    return TRUE;
}

BOOL OpenReadFileUtf8(CONST LPCSTR lpFileName, LPSTR* fileString, CONST LPDWORD fileLength)
{
    HANDLE hFile = NULL;
    if (!OpenFileRead(lpFileName, &hFile))
        return FALSE;

    BOOL success = ReadFileUtf8(hFile, fileString, fileLength);
    CloseHandle(hFile);

    return success;
}

BOOL StrEndsWith(CONST LPSTR str, CONST LPSTR target)
{
    DWORD strLen = lstrlenA(str), targetLen = lstrlenA(target);

    if (strLen < targetLen)
        return FALSE;

    return lstrcmpA(str + strLen - targetLen, target) == 0;
}

LPSTR SkipBomUtf8(CONST LPSTR str)
{
    if (str[0] == (char)0xEF
     && str[1] == (char)0xBB
     && str[2] == (char)0xBF)
    {
        return str + 3;
    }

    return str;
}

INT TerminateLine(CONST LPSTR str)
{
    for (INT i = 0;; i++)
    {
        if (str[i] == '\0')
            return i + 1;

        if (str[i] == '\r' || str[i] == '\n')
        {
            str[i] = '\0';

            if (str[i + 1] == '\n')
            {
                str[++i] = '\0';
            }

            return i + 1;
        }
    }
}