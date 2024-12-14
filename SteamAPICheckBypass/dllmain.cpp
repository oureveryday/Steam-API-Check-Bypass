#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "lib_main/lib_main.hpp"
#include "Helpers/Helpers.hpp"
#include "Configs/Configs.hpp"

#include <filesystem>
#include <string>

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

std::vector<std::wstring> initial_files = {
    // the current name of the dll is added here as a first entry
    L"steamapicheckbypass.json",
    L"steamapicheckbypass_config.json",
    L"SteamAPICheckBypass.json",
    L"SteamAPICheckBypass_congig.json",
    L"nt_file_dupe.json",
    L"nt_file_dupe_config.json",
    L"nt_fs_dupe.json",
    L"nt_fs_dupe_config.json",
    L"nt_dupe.json",
    L"nt_dupe_config.json",
};

void add_original_entries(const std::wstring& original_path) {
    std::wstring _original_path = ntfsdupe::helpers::to_absolute(original_path, ntfsdupe::cfgs::get_exe_dir());
    std::filesystem::path path(_original_path);
    std::wstring extension = path.extension().wstring();
    std::wstring stem = path.stem().wstring();
    std::wstring parent_path = path.parent_path().wstring();

    ntfsdupe::cfgs::add_entry(ntfsdupe::cfgs::Mode::file_redirect, _original_path, parent_path + L"\\" + stem + extension + L".bak", true);
    ntfsdupe::cfgs::add_entry(ntfsdupe::cfgs::Mode::file_redirect, _original_path, parent_path + L"\\" + stem + L".org", true);
    ntfsdupe::cfgs::add_entry(ntfsdupe::cfgs::Mode::file_redirect, _original_path, parent_path + L"\\" + stem + L"_o" + extension, true);
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        if (!ntfsdupe::init()) return FALSE;

        std::wstring my_path_str(ntfsdupe::helpers::get_module_fullpath(hModule));
        if (my_path_str.empty()) return FALSE;

        // hide ourself (on disk)
        if (!ntfsdupe::cfgs::add_entry(ntfsdupe::cfgs::Mode::file_hide, my_path_str)) return FALSE;

        auto my_path = std::filesystem::path(my_path_str);

        // hide ourself (in memory)
        //if (!ntfsdupe::cfgs::add_entry(ntfsdupe::cfgs::Mode::module_hide_handle, my_path.filename().wstring())) return FALSE;

        std::wstring stem_lower = my_path.stem().wstring();
        std::transform(stem_lower.begin(), stem_lower.end(), stem_lower.begin(), ::towlower);
        if (stem_lower != L"version") {
            if (!ntfsdupe::cfgs::add_entry(ntfsdupe::cfgs::Mode::module_hide_handle, stem_lower)) return FALSE;
        }
        
        // add <dll name>.json to the list
        initial_files.insert(initial_files.begin(), my_path.stem().wstring() + L".json");

        // try to load some files by default
        auto my_dir = my_path.parent_path();
        bool found_cfg_file = false;
        for (const auto &file : initial_files) {
            auto cfg_file = (my_dir / file).wstring();
            if (ntfsdupe::cfgs::load_file(cfg_file.c_str())) {
                found_cfg_file = true;
                // hiding this file isn't really critical, right?
                ntfsdupe::cfgs::add_entry(ntfsdupe::cfgs::Mode::file_hide, cfg_file);
                break;
            }
        }

        if (!found_cfg_file)
        {
            // hide exe
            add_original_entries(ntfsdupe::helpers::get_module_fullpath(nullptr));

            // hide steam_api.dll
            add_original_entries(L"steam_api.dll");

            // hide steam_api64.dll
            add_original_entries(L"steam_api64.dll");
        }

    }
    break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    break;

    case DLL_PROCESS_DETACH:
        ntfsdupe::deinit();
    break;
    }

    return TRUE;
}

