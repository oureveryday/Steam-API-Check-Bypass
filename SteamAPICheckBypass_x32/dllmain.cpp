#include "pch.h"

#include <codecvt>

#include "Console.h"
#include "detours.h"
#include <fstream>
#include <intrin.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <winternl.h>

#pragma comment(lib, "detours.lib")
#pragma comment(lib, "ntdll.lib")

struct Replace
{
	std::wstring origname;
	std::wstring replacename;
	bool replaceafterfirsttime;    // Replace reading request after reading for first time
	bool firstime = false;        // first time read indicator, should always be false
};

//----------Configuration start---------------

bool useinternallist = false;   //Use built-in replace list without reading .ini file

bool debugprintpath = false;    //Print the path of the file being read

bool enabledebuglogfile = false;      //Enable debug log file

std::string logfilename = "SteamAPICheckBypass.log"; //Log file name

std::wstring inifilename = L"\\SteamAPICheckBypass.ini"; //Ini file name

std::vector<Replace> internalreplaceList = {
		{L"steam_api.dll", L"steam_api.org", false, false},
		{L"steam_api64.dll", L"steam_api64.org", false, false},
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
	std::string logstr = "[SteamAPICheckBypass] " + str + "\n";
#ifdef _DEBUG
	Console::Print(logstr.c_str());
	if (enabledebuglogfile)
	{
		std::ofstream logfile;
		logfile.open(logfilename, std::ios_base::app);
		logfile << logstr;
	}
#endif
	int wideStrLength = MultiByteToWideChar(CP_UTF8, 0, logstr.c_str(), -1, nullptr, 0);
	wchar_t* wideString = new wchar_t[wideStrLength];
	MultiByteToWideChar(CP_UTF8, 0, logstr.c_str(), -1, wideString, wideStrLength);
	OutputDebugString(wideString);
	delete[] wideString;
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

std::wstring utf8ToUtf16(const std::string& utf8Str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(utf8Str);
}

std::string utf16ToUtf8(const std::wstring& utf16Str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.to_bytes(utf16Str);
}

typedef enum _SECTION_INFORMATION_CLASS {
	SectionBasicInformation,
	SectionImageInformation
} SECTION_INFORMATION_CLASS, * PSECTION_INFORMATION_CLASS;
EXTERN_C NTSTATUS __stdcall NtQuerySection(HANDLE SectionHandle, SECTION_INFORMATION_CLASS InformationClass, PVOID InformationBuffer, ULONG InformationBufferSize, PULONG ResultLength);
EXTERN_C NTSTATUS __stdcall NtProtectVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, PULONG  NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
EXTERN_C NTSTATUS __stdcall NtPulseEvent(HANDLE EventHandle, PULONG PreviousState);

void DisableVMP()
{
	// restore hook at NtProtectVirtualMemory
	auto ntdll = GetModuleHandleA("ntdll.dll");
	if (ntdll == NULL) return;

	bool linux = GetProcAddress(ntdll, "wine_get_version") != nullptr;
	void* routine = linux ? (void*)NtPulseEvent : (void*)NtQuerySection;
	DWORD old;
	VirtualProtect(NtProtectVirtualMemory, 1, PAGE_EXECUTE_READWRITE, &old);
	*(uintptr_t*)NtProtectVirtualMemory = *(uintptr_t*)routine & ~(0xFFui64 << 32) | (uintptr_t)(*(uint32_t*)((uintptr_t)routine + 4) - 1) << 32;
	VirtualProtect(NtProtectVirtualMemory, 1, old, &old);
}
#pragma endregion

std::vector<Replace> replaceList;

typedef NTSTATUS(WINAPI* pNtCreateFile)(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer,
	ULONG EaLength);

pNtCreateFile oNtCreateFile = nullptr;

typedef NTSTATUS(WINAPI* pNtOpenFile)(
	PHANDLE            FileHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK   IoStatusBlock,
	ULONG              ShareAccess,
	ULONG              OpenOptions);

pNtOpenFile oNtOpenFile = nullptr;

std::wstring GetReplacedPath(std::wstring path)
{
	// Get the file name from the path
	size_t lastSlash = path.find_last_of('/');
	size_t lastBackslash = path.find_last_of('\\');
	size_t lastSeparator = (lastSlash > lastBackslash) ? lastSlash : lastBackslash;
	std::wstring filename = path.substr(lastSeparator + 1);
	if (filename.find(utf8ToUtf16(logfilename)) == std::string::npos && debugprintpath)
	{
		PrintLog("Reading Path:" + utf16ToUtf8(path));
	}
	// Check if the file name matches any entry in the replaceList
	for (Replace& replace : replaceList)
	{
		if (filename.find(replace.origname) != std::wstring::npos)
		{

			replace.firstime = true;

			if (replace.replaceafterfirsttime && !replace.firstime)
			{
				PrintLog("Reading " + utf16ToUtf8(replace.origname) + "for first time.");
				break;
			}
			PrintLog("Reading " + utf16ToUtf8(replace.origname) + ",Replacing...");
			// Replace the path's filename with replacename
			size_t pos = path.find_last_of(L"/\\");
			path = path.substr(0, pos + 1) + replace.replacename;
			PrintLog("Replaced Path:" + utf16ToUtf8(path));
			// Set firstime to true if replaceafterfirsttime is true and firstime is false


			break; // No need to check further once a replacement is made
		}
	}

	return path;
}

NTSTATUS WINAPI NtCreateFileHook(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer,
	ULONG EaLength)
{
	try
	{
		if (ObjectAttributes != nullptr && ObjectAttributes->ObjectName &&
			ObjectAttributes->ObjectName->Length &&
			ObjectAttributes->ObjectName->Buffer != nullptr && !IsBadReadPtr(ObjectAttributes->ObjectName->Buffer, sizeof(WCHAR)) && ObjectAttributes->ObjectName->Buffer[0]) {
			std::wstring originalPath(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length / sizeof(WCHAR));
			std::wstring replacedPathStr = GetReplacedPath(originalPath);
			UNICODE_STRING replacedPathUnicode;
			RtlInitUnicodeString(&replacedPathUnicode, replacedPathStr.c_str());
			ObjectAttributes->ObjectName = &replacedPathUnicode;
			return oNtCreateFile(
				FileHandle,
				DesiredAccess,
				ObjectAttributes,
				IoStatusBlock,
				AllocationSize,
				FileAttributes,
				ShareAccess,
				CreateDisposition,
				CreateOptions,
				EaBuffer,
				EaLength);
		}
	}
	catch (...)
	{
		PrintLog("Error in NtCreateFileHook");
	}
	
	return oNtCreateFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer,
		EaLength);
}

NTSTATUS WINAPI NtOpenFileHook(
	PHANDLE            FileHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK   IoStatusBlock,
	ULONG              ShareAccess,
	ULONG              OpenOptions)
{
	try
	{
		if (ObjectAttributes != nullptr && ObjectAttributes->ObjectName &&
			ObjectAttributes->ObjectName->Length &&
			ObjectAttributes->ObjectName->Buffer != nullptr && !IsBadReadPtr(ObjectAttributes->ObjectName->Buffer, sizeof(WCHAR)) && ObjectAttributes->ObjectName->Buffer[0]) {
			std::wstring originalPath(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length / sizeof(WCHAR));
			std::wstring replacedPathStr = GetReplacedPath(originalPath);
			UNICODE_STRING replacedPathUnicode;
			RtlInitUnicodeString(&replacedPathUnicode, replacedPathStr.c_str());
			ObjectAttributes->ObjectName = &replacedPathUnicode;
			return oNtOpenFile(
				FileHandle,
				DesiredAccess,
				ObjectAttributes,
				IoStatusBlock,
				ShareAccess,
				OpenOptions);
		}
	}
	catch (...)
	{
		PrintLog("Error in NtOpenFileHook");
	}
	
	return oNtOpenFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		ShareAccess,
		OpenOptions);
}

void LoadHook()
{
	PrintLog("Starting to hook File APIs...");
	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (hNtdll)
	{
		oNtCreateFile = (pNtCreateFile)GetProcAddress(hNtdll, "NtCreateFile");
		if (oNtCreateFile)
		{
			DetourAttach(&(PVOID&)oNtCreateFile, NtCreateFileHook);
			auto Error = DetourTransactionCommit();
			if (Error == NO_ERROR)
				PrintLog("Hooked NtCreateFile");
			else
				PrintLog("NtCreateFile Hook Failed. Error: " + std::to_string(Error));
		}
		else
		{
			PrintLog("NtCreateFile Hook Failed. Error: Failed to get NtCreateFile address.");
		}
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (hNtdll)
	{
		oNtOpenFile = (pNtOpenFile)GetProcAddress(hNtdll, "NtOpenFile");
		if (oNtOpenFile)
		{
			DetourAttach(&(PVOID&)oNtOpenFile, NtOpenFileHook);
			auto Error = DetourTransactionCommit();
			if (Error == NO_ERROR)
				PrintLog("Hooked NtOpenFile");
			else
				PrintLog("NtOpenFile Hook Failed. Error: " + std::to_string(Error));
		}
		else
		{
			PrintLog("NtOpenFile Hook Failed. Error: Failed to get NtOpenFile address.");
		}
	}
}

bool readReplacesFromIni(const std::wstring& filename, std::vector<Replace>& replaceList) {
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
		replace.origname = utf8ToUtf16(entry.first);
		replace.replacename = utf8ToUtf16(entry.second);
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
		replaceList.push_back({ L"steam_api.dll",L"steam_api.dll.bak",false });
	}
	else if (SteamAPI_ORG)
	{
		PrintLog("Found steam_api.org.");
		replaceList.push_back({ L"steam_api.dll",L"steam_api.org",false });
	}
	else if (SteamAPI_O)
	{
		PrintLog("Found steam_api_o.dll.");
		replaceList.push_back({ L"steam_api.dll",L"steam_api_o.dll",false });
	}

	if (SteamAPI64_BAK)
	{
		PrintLog("Found steam_api64.dll.bak.");
		replaceList.push_back({ L"steam_api.dll",L"steam_api64.dll.bak",false });
	}
	else if (SteamAPI64_ORG)
	{
		PrintLog("Found steam_api64.org.");
		replaceList.push_back({ L"steam_api.dll",L"steam_api64.org",false });
	}
	else if (SteamAPI64_O)
	{
		PrintLog("Found steam_api64_o.dll.");
		replaceList.push_back({ L"steam_api.dll",L"steam_api64_o.dll",false });
	}
}

void GetReplaceList()
{
	wchar_t iniPath[MAX_PATH];
	wcscpy_s(iniPath, MAX_PATH, GetCurrentPath());
	wcscat_s(iniPath, MAX_PATH, inifilename.c_str());

	if (useinternallist)
	{
		replaceList = internalreplaceList;
	}

	if (readReplacesFromIni(iniPath, replaceList))
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
			PrintLog(utf16ToUtf8(replace.origname) + "," + utf16ToUtf8(replace.replacename));
		}
	}
	PrintLog("-----------------");
}

void Init()
{
	PrintLog("SteamAPICheckBypass Init");
	GetReplaceList();
	std::thread([]() {
		DisableVMP();
		LoadHook();
		}).detach();
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
#ifdef _DEBUG
	Console::Attach();
#endif

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
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