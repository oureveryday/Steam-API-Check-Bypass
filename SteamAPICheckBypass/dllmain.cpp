#include "pch.h"

#include <algorithm>
#include <codecvt>

#include "detours.h"
#include "Console.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>


#pragma comment(lib, "detours.lib")
#pragma comment(lib, "ntdll.lib")

struct Replace
{
	std::string origname;
	std::string replacename;
	bool replaceafterfirsttime;    // Replace reading request after reading for first time
	bool firstime = false;        // first time read indicator, should always be false
};

//----------Configuration start---------------

bool useinternallist = false;   //Use built-in replace list without reading .ini file

std::vector<Replace> internalreplaceList = {
		{"steam_api.dll", "steam_api.org", false, false},
		{"steam_api64.dll", "steam_api64.org", false, false},
};//internal replace list example

//----------Configuration end-----------------


#pragma region Utils
bool isFileExist(const wchar_t* fileName) {
	HANDLE hFile = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return true;
	}
	return false;
}


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
std::string convertWCharToString(const wchar_t* wideString) {
	// Get the length of the converted string
	int size = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, NULL, 0, NULL, NULL);

	// Allocate a buffer for the converted string
	char* buffer = new char[size];

	// Convert the wchar_t string to a char string
	WideCharToMultiByte(CP_UTF8, 0, wideString, -1, buffer, size, NULL, NULL);

	// Create a std::string from the char string
	std::string convertedString(buffer);

	// Clean up the allocated buffer
	delete[] buffer;

	return convertedString;
}

wchar_t const* GetCurrentPath()
{
	wchar_t exePath[MAX_PATH];
	GetModuleFileNameW(NULL, exePath, MAX_PATH);
	wchar_t* lastBackslash = wcsrchr(exePath, L'\\');
	if (lastBackslash != nullptr) {
		*lastBackslash = L'\0';  // Null-terminate to get the directory path
	}
	return exePath;
}
#pragma endregion

std::vector<Replace> replaceList;

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

std::string GetReplacedPath(std::string path)
{
	// Get the file name from the path
	size_t lastSlash = path.find_last_of('/');
	size_t lastBackslash = path.find_last_of('\\');
	size_t lastSeparator = (lastSlash > lastBackslash) ? lastSlash : lastBackslash;
	std::string filename = path.substr(lastSeparator + 1);

	// Check if the file name matches any entry in the replaceList
	for (Replace& replace : replaceList)
	{
		if (filename.find(replace.origname) != std::string::npos)
		{
			
			replace.firstime = true;

			if (replace.replaceafterfirsttime && !replace.firstime)
			{
				PrintLog("Reading " + replace.origname + "for first time.");
				break;
			}
			PrintLog("Reading " + replace.origname + ",Replacing...");
			// Replace the path's filename with replacename
			size_t pos = path.find_last_of("/\\");
			path = path.substr(0, pos + 1) + replace.replacename;
			PrintLog(path);
			// Set firstime to true if replaceafterfirsttime is true and firstime is false
			

			break; // No need to check further once a replacement is made
		}
	}

	return path;
}

HANDLE WINAPI CreateFileAHook(
	LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	std::string origpath = std::string(lpFileName);
	std::string newFileNamestring = GetReplacedPath(origpath);
	LPCSTR newFileName = newFileNamestring.c_str();

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
	size_t len = wcslen(lpFileName) + 1;
	size_t convertedChars = 0;
	char* mbstr = new char[len];
	wcstombs_s(&convertedChars, mbstr, len, lpFileName, len - 1);
	std::string origpath = std::string(mbstr);
	std::string newFileName = GetReplacedPath(origpath);
	std::wstring newFileNamestring = std::wstring(newFileName.begin(), newFileName.end());
	LPCWSTR newFileNameLPC=newFileNamestring.c_str();

	return OrigCreateFileW(newFileNameLPC, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
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

LPCWSTR ConvertToLPCWSTR(const char* charString) {
	int size = MultiByteToWideChar(CP_UTF8, 0, charString, -1, nullptr, 0);
	wchar_t* buffer = new wchar_t[size];
	MultiByteToWideChar(CP_UTF8, 0, charString, -1, buffer, size);
	return buffer;
}

LPWSTR ConvertToLPWSTR(const char* charString) {
	int size = MultiByteToWideChar(CP_UTF8, 0, charString, -1, nullptr, 0);
	wchar_t* buffer = new wchar_t[size];
	MultiByteToWideChar(CP_UTF8, 0, charString, -1, buffer, size);
	return buffer;
}

bool readReplacesFromIni(const std::string& filename, std::vector<Replace>& replaceList) {
	std::ifstream iniFile(filename);
	std::string line;
	std::map<std::string, std::string> replaceMap;
	std::map<std::string, bool> afterFirstTimeMap;
	if (!iniFile.is_open()) {
		PrintLog("Unable to open ini file.");
		return false;
	}

	
	while (std::getline(iniFile, line) && line != "[AfterFirstTime]") {
		if (line[0] == '[') continue; 
		std::istringstream is_line(line);
		std::string key;
		if (std::getline(is_line, key, '=')) {
			std::string value;
			if (std::getline(is_line, value)) {
				replaceMap[key] = value;
			}
		}
	}

	while (std::getline(iniFile, line)) {
		if (line[0] == '[') continue;
		std::istringstream is_line(line);
		std::string key;
		if (std::getline(is_line, key, '=')) {
			std::string value;
			if (std::getline(is_line, value)) {
				if (value == "1")
				{
					afterFirstTimeMap[key] = true;
				}
				else afterFirstTimeMap[key] = false;
			}
		}
	}


	for (const auto& entry : replaceMap) {
		Replace replace;
		replace.origname = entry.first;
		replace.replacename = entry.second;
		replace.replaceafterfirsttime = afterFirstTimeMap[entry.first];
		replaceList.push_back(replace);
	}

	iniFile.close();
	return true;
}

void Checkfile(std::vector<Replace>& replaceList)
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
		replaceList.push_back({ "steam_api.dll","steam_api.dll.bak",false });
	}else if (SteamAPI_ORG)
	{
		PrintLog("Found steam_api.org.");
		replaceList.push_back({ "steam_api.dll","steam_api.org",false });
	}else if (SteamAPI_O)
	{
		PrintLog("Found steam_api_o.dll.");
		replaceList.push_back({ "steam_api.dll","steam_api_o.dll",false });
	}

	if (SteamAPI64_BAK)
	{
		PrintLog("Found steam_api64.dll.bak.");
		replaceList.push_back({ "steam_api.dll","steam_api64.dll.bak",false });
	}else if (SteamAPI64_ORG)
	{
		PrintLog("Found steam_api64.org.");
		replaceList.push_back({ "steam_api.dll","steam_api64.org",false });
	}else if (SteamAPI64_O)
	{
		PrintLog("Found steam_api64_o.dll.");
		replaceList.push_back({ "steam_api.dll","steam_api64_o.dll",false });
	}
}

void GetReplaceList()
{
	wchar_t iniPath[MAX_PATH];
	wcscpy_s(iniPath, MAX_PATH, GetCurrentPath());
	wcscat_s(iniPath, MAX_PATH, L"\\SteamAPICheckBypass.ini");

	if(useinternallist)
	{
		replaceList = internalreplaceList;
	}

	if(readReplacesFromIni(convertWCharToString(iniPath), replaceList))
	{
		PrintLog("Successfully get ini replace infos.");
	}
	else
	{
		PrintLog("Failed to get ini replace infos, detecting files...");
		Checkfile(replaceList);
	}
	PrintLog("-----------------");
	PrintLog("Replace List:");
	{
		for (const auto& replace : replaceList) {
			PrintLog(replace.origname + "," + replace.replacename);
		}
	}
	PrintLog("-----------------");
}

void Init()
{
	PrintLog("SteamAPICheckBypass Init");
	GetReplaceList();
	LoadHook();
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

#ifdef _DEBUG
		Console::Attach();
#endif

		PrintLog("Steam API Check Bypass dll Loaded.");
		Init();

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void SteamAPICheckBypass() {};