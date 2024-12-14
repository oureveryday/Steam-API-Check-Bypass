#pragma once

#include <cctype>
#include <string>
#include <algorithm>
#include <memory>
#include <filesystem>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define unique_ptr_stack(Type, size) \
    std::unique_ptr<Type, void (*)(void* const)>( \
        (Type*)_malloca(size), \
        [](void* const p) -> void { _freea(p); })

#define null_unique_ptr_stack(Type) \
    std::unique_ptr<Type, void (*)(void* const)>(nullptr, nullptr)

namespace ntfsdupe::helpers
{
    __forceinline static std::wstring upper(const std::wstring& wstr)
    {
        std::wstring _wstr(wstr);
        std::transform(
            _wstr.begin(), _wstr.end(), _wstr.begin(),
            [](auto c) { return c == L'/' ? L'\\' : std::toupper(c); }
        );

        return _wstr;
    }

    __forceinline static std::wstring str_to_wstr(const std::string &str)
    {
        if (str.empty()) return {};

        int chars = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
        if (!chars) return {};

        std::wstring wstr(chars, 0);
        chars = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], (int)wstr.size());
        if (!chars) return {};

        return wstr;
    }

    __forceinline static bool file_exist(const std::filesystem::path &filepath)
    {
        if (std::filesystem::is_directory(filepath)) return false;
        else if (std::filesystem::exists(filepath)) return true;
        
        return false;
    }

    __forceinline static bool file_exist(const std::string &filepath)
    {
        if (filepath.empty()) return false;
        
        return file_exist(std::filesystem::u8path(filepath));
    }

    __forceinline static bool file_exist(const std::wstring &filepath)
    {
        if (filepath.empty()) return false;

        return file_exist(std::filesystem::path(filepath));
    }

    __forceinline static std::filesystem::path to_absolute(const std::filesystem::path &path, const std::filesystem::path &base)
    {
        if (path.is_absolute()) return path;

        return std::filesystem::absolute(base / path);
    }

    __forceinline static std::wstring to_absolute(const std::wstring& path, const std::wstring& base)
    {
        if (path.empty()) return path;
        auto path_abs = to_absolute(
            std::filesystem::path(path),
            base.empty() ? std::filesystem::current_path() : std::filesystem::path(base)
        );
        return path_abs.wstring();
    }

    __forceinline static wchar_t* upper(wchar_t* wstr, int count)
    {
        if (!wstr || !wstr[0] || !count) return wstr;

        while (count) {
            *wstr = (wchar_t)std::toupper(*wstr);
            wstr++;
            count--;
        }
        return wstr;
    }
    
    __forceinline static std::wstring get_module_fullpath(HMODULE hModule)
    {
        size_t chars = 512;
        std::wstring mod_path{};
        do {
            chars *= 2;
            mod_path.resize(chars);
            // from docs:
            // The string returned will use the same format that was specified when the module was loaded.
            // Therefore, the path can be a long or short file name, and can use the prefix "\?"
            chars = GetModuleFileNameW(hModule, &mod_path[0], (DWORD)mod_path.size());
            if (!chars) return {};
        } while (chars == mod_path.size());
        if (!chars) return {};

        return mod_path.substr(0, chars);
    }

    __forceinline static std::wstring get_current_module_fullpath()
    {
        HMODULE hModule = GetModuleHandleW(nullptr);
        if (!hModule) return {};
        return get_module_fullpath(hModule);
    }

    __forceinline static std::wstring get_module_name(HMODULE hModule)
    {
        std::wstring full_path = get_module_fullpath(hModule);
        if (full_path.empty()) return {};

        std::filesystem::path path(full_path);
        return path.filename().wstring();
    }
}
