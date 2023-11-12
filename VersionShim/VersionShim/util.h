#pragma once

UINT ErrBoxType = MB_ICONERROR | MB_TOPMOST;

BOOL OpenReadFileUtf8(LPCSTR lpFileName, LPSTR* fileBuffer, LPDWORD fileLength);
BOOL StrEndsWith(LPSTR str, LPSTR target);
LPSTR SkipBomUtf8(LPSTR str);
INT TerminateLine(LPSTR str);