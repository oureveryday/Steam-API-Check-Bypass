#include "pch.h"

#include <algorithm>
#include <codecvt>

#include "detours.h"
#include "Console.h"
#include <winternl.h>
#include <Windows.h>
#include <intrin.h>
#include <iostream>
#include <string>

#pragma comment(lib, "detours.lib")
#pragma comment(lib, "ntdll.lib")

#pragma region Utils

wchar_t steamAPI_Path[MAX_PATH];
wchar_t steamAPI64_Path[MAX_PATH];
bool steamAPI_P = false;
bool steamAPI64_P = false;

void PrintLog(std::string str)
{
#ifdef _DEBUG
	Console::Print(("[SteamAPICheckBypass] " + str + "\n").c_str());
#endif
	std::string logstr = "[SteamAPICheckBypass] " + str + "\n";
	int wideStrLength = MultiByteToWideChar(CP_UTF8, 0, logstr.c_str(), -1, nullptr, 0);
	wchar_t* wideString = new wchar_t[wideStrLength];
	MultiByteToWideChar(CP_UTF8, 0, logstr.c_str(), -1, wideString, wideStrLength);
	OutputDebugString(wideString);
	delete[] wideString;
}

HANDLE(WINAPI* OrigCreateFileW) (
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile) = CreateFileW;

HANDLE(WINAPI* OrigCreateFileA) (
	LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile) = CreateFileA;


HANDLE WINAPI CreateFileAHook(
	LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	LPCSTR newFileName;

	std::string origpath = std::string(lpFileName);
	std::transform(origpath.begin(), origpath.end(), origpath.begin(), tolower);
	if ((std::string::npos != origpath.find("steam_api.dll")) && steamAPI_P)
	{
		PrintLog("Detected reading steam_api.dll at CreateFileA, replacing...");
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, steamAPI_Path, -1, NULL, 0, NULL, NULL);
		LPSTR ansiString = new char[bufferSize];
		WideCharToMultiByte(CP_UTF8, 0, steamAPI_Path, -1, ansiString, bufferSize, NULL, NULL);
		newFileName = ansiString;
	}
	else if ((std::string::npos != origpath.find("steam_api64.dll")) && steamAPI64_P)
	{
		PrintLog("Detected reading steam_api64.dll at CreateFileA, replacing...");
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, steamAPI64_Path, -1, NULL, 0, NULL, NULL);
		LPSTR ansiString = new char[bufferSize];
		WideCharToMultiByte(CP_UTF8, 0, steamAPI64_Path, -1, ansiString, bufferSize, NULL, NULL);
		newFileName = ansiString;
	}
	else
	{
		newFileName = lpFileName;
	}

	return OrigCreateFileA(newFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI CreateFileWHook(
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	LPCWSTR newFileName;

	size_t len = wcslen(lpFileName) + 1;
	size_t convertedChars = 0;
	char* mbstr = new char[len];
	wcstombs_s(&convertedChars, mbstr, len, lpFileName, len - 1);
	std::string origpath = std::string(mbstr);
	delete[] mbstr;
	std::transform(origpath.begin(), origpath.end(), origpath.begin(), tolower);
	if ((std::string::npos != origpath.find("steam_api.dll")) && steamAPI_P)
	{
		PrintLog("Detected reading steam_api.dll at CreateFileW, replacing...");
		newFileName = steamAPI_Path;
	}
	else if ((std::string::npos != origpath.find("steam_api64.dll")) && steamAPI64_P)
	{
		PrintLog("Detected reading steam_api64.dll at CreateFileW, replacing...");
		newFileName = steamAPI64_Path;
	}
	else
	{
		newFileName = lpFileName;
	}

	return OrigCreateFileW(newFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


void LoadHook()
{
	PrintLog("Starting to hook CreateFile APIs...");

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)OrigCreateFileA, CreateFileAHook);
	auto Error = DetourTransactionCommit();

	if (Error == NO_ERROR)
		PrintLog("Hooked CreateFileA");
	else
		PrintLog("CreateFileA Hook Failed.");

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)OrigCreateFileW, CreateFileWHook);
	Error = DetourTransactionCommit();

	if (Error == NO_ERROR)
		PrintLog("Hooked CreateFileW");
	else
		PrintLog("CreateFileW Hook Failed.");
}

bool isFileExist(const wchar_t* fileName) {
	HANDLE hFile = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return true;
	}
	return false;
}

void Checkfile()
{
	PrintLog("Checking Original Steam_API(64) files...");

	wchar_t exePath[MAX_PATH];
	GetModuleFileNameW(NULL, exePath, MAX_PATH);

	// Extract the directory from the full path
	wchar_t* lastBackslash = wcsrchr(exePath, L'\\');
	if (lastBackslash != nullptr) {
		*lastBackslash = L'\0';  // Null-terminate to get the directory path
	}

	// Construct file paths based on the executable path
	wchar_t steamAPI_ORG[MAX_PATH];
	wcscpy_s(steamAPI_ORG, MAX_PATH, exePath);
	wcscat_s(steamAPI_ORG, MAX_PATH, L"\\steam_api.org");

	wchar_t steamAPI_BAK[MAX_PATH];
	wcscpy_s(steamAPI_BAK, MAX_PATH, exePath);
	wcscat_s(steamAPI_BAK, MAX_PATH, L"\\steam_api.dll.bak");

	wchar_t steamAPI_O[MAX_PATH];
	wcscpy_s(steamAPI_O, MAX_PATH, exePath);
	wcscat_s(steamAPI_O, MAX_PATH, L"\\steam_api_o.dll");

	wchar_t steamAPI64_ORG[MAX_PATH];
	wcscpy_s(steamAPI64_ORG, MAX_PATH, exePath);
	wcscat_s(steamAPI64_ORG, MAX_PATH, L"\\steam_api64.org");

	wchar_t steamAPI64_BAK[MAX_PATH];
	wcscpy_s(steamAPI64_BAK, MAX_PATH, exePath);
	wcscat_s(steamAPI64_BAK, MAX_PATH, L"\\steam_api64.dll.bak");

	wchar_t steamAPI64_O[MAX_PATH];
	wcscpy_s(steamAPI64_O, MAX_PATH, exePath);
	wcscat_s(steamAPI64_O, MAX_PATH, L"\\steam_api64_o.dll");


	bool SteamAPI_ORG = isFileExist(steamAPI_ORG);
	bool SteamAPI_BAK = isFileExist(steamAPI_BAK);
	bool SteamAPI_O = isFileExist(steamAPI_O);
	bool SteamAPI64_ORG = isFileExist(steamAPI64_ORG);
	bool SteamAPI64_BAK = isFileExist(steamAPI64_BAK);
	bool SteamAPI64_O = isFileExist(steamAPI64_O);

	if (SteamAPI_BAK)
	{
		PrintLog("Found steam_api.dll.bak.");
		steamAPI_P = true;
		wcscpy_s(steamAPI_Path, MAX_PATH, steamAPI_BAK);
	}

	if (SteamAPI_ORG)
	{
		PrintLog("Found steam_api.org.");
		steamAPI_P = true;
		wcscpy_s(steamAPI_Path, MAX_PATH, steamAPI_ORG);
	}

	if (SteamAPI_O)
	{
		PrintLog("Found steam_api_o.dll.");
		steamAPI_P = true;
		wcscpy_s(steamAPI_Path, MAX_PATH, steamAPI_O);
	}

	if (SteamAPI64_BAK)
	{
		PrintLog("Found steam_api64.dll.bak.");
		steamAPI64_P = true;
		wcscpy_s(steamAPI64_Path, MAX_PATH, steamAPI64_BAK);
	}

	if (SteamAPI64_ORG)
	{
		PrintLog("Found steam_api64.org.");
		steamAPI64_P = true;
		wcscpy_s(steamAPI64_Path, MAX_PATH, steamAPI64_ORG);
	}

	if (SteamAPI64_O)
	{
		PrintLog("Found steam_api64_o.dll.");
		steamAPI_P = true;
		wcscpy_s(steamAPI_Path, MAX_PATH, steamAPI64_O);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	LONG Error;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

#ifdef _DEBUG
		Console::Attach();
#endif

		PrintLog("Steam API Check Bypass dll Loaded.");
		Checkfile();
		LoadHook();

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void SteamAPICheckBypass() {};