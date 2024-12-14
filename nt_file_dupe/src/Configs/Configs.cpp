#include "Helpers/Helpers.hpp"
#include "Helpers/dbglog.hpp"
#include "Json/nlohmann/json.hpp"
#include "NtApis/NtApis.hpp"
#include "NtApis/peb_helper.hpp"
#include "Configs/Configs.hpp"

#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <filesystem>


namespace ntfsdupe::cfgs {
    using json = nlohmann::json;

    static CRITICAL_SECTION bypass_files_cs{};
    static std::wstring exe_dir{};
    // this storage holds all strings used (actual memory allocation), which are later referenced by the other containers
    // it is way cheaper to use string_view for other containers since it doesn't copy strings, and Ntxxx APIs use
    // strings with length, so just wrapping them is enough
    static std::unordered_set<std::wstring> global_entries_storage{};
    static std::unordered_map<std::wstring_view, ntfsdupe::cfgs::FileCfgEntry> file_entries{};
    static std::unordered_map<std::wstring_view, ntfsdupe::cfgs::ModuleCfgEntry> module_entries{};
    static std::unordered_map<DWORD, std::unordered_map<std::wstring_view, size_t>> bypass_entries{};

    bool add_entry_file(Mode mode, const std::wstring& original, const std::wstring& target = std::wstring(), bool file_must_exist = false, HookTimesMode hook_times_cfg = HookTimesMode::all, int hook_time_n = 0);
    bool add_entry_module(Mode mode, const std::wstring& original, const std::wstring& target = std::wstring());
}


bool ntfsdupe::cfgs::init()
{
    NTFSDUPE_DBG(L"ntfsdupe::cfgs::init()");

    InitializeCriticalSection(&bypass_files_cs);

    exe_dir.clear();
    file_entries.clear();
    bypass_entries.clear();
    
    HMODULE hModule = GetModuleHandleW(nullptr);
    if (!hModule) {
        NTFSDUPE_DBG(L"  couldn't get module handle");
        return false;
    }

    exe_dir = ntfsdupe::helpers::get_module_fullpath(hModule);
    if (exe_dir.empty()) {
        NTFSDUPE_DBG(L"  couldn't get exe dir");
        return false;
    }

    // hide ourself
    // add_entry(Mode::hide, lib_dir);

    exe_dir = exe_dir.substr(0, exe_dir.find_last_of(L'\\') + 1);
    return true;
}

void ntfsdupe::cfgs::deinit()
{
    NTFSDUPE_DBG(L"ntfsdupe::cfgs::deinit()");
    
    DeleteCriticalSection(&bypass_files_cs);
    memset(&bypass_files_cs, 0, sizeof(bypass_files_cs));
}

const std::wstring& ntfsdupe::cfgs::get_exe_dir() noexcept
{
    return exe_dir;
}

bool ntfsdupe::cfgs::add_entry_file(Mode mode, const std::wstring& original, const std::wstring& target, bool file_must_exist, HookTimesMode hook_times_cfg, int hook_time_n)
{
    std::wstring _original = ntfsdupe::helpers::to_absolute(original, exe_dir);
    NTFSDUPE_DBG(L"  absolute original file: '%s'", _original.c_str());
    if (file_must_exist && !std::filesystem::exists(_original)) {  // not a problem
        NTFSDUPE_DBG(L"  original file not found");
        return true;
    }

    size_t filename_idx = _original.find_last_of(L'\\') + 1;
    unsigned short filename_bytes = (unsigned short)((_original.size() - filename_idx) * sizeof(_original[0]));
    NTFSDUPE_DBG(L"  filename_idx = %zu, filename_bytes = %u", filename_idx, filename_bytes); // TODO

    std::wstring _target = target.empty() ? target : ntfsdupe::helpers::to_absolute(target, exe_dir);
    NTFSDUPE_DBG(L"  absolute target file: '%s'", _target.c_str());

    switch (mode) {
    case ntfsdupe::cfgs::Mode::file_hide: {
        const auto& _original_upper_ref = *global_entries_storage.insert(ntfsdupe::helpers::upper(_original)).first;
        FileCfgEntry& entry = file_entries[_original_upper_ref];
        entry.mode = FileType::hide;
        entry.original = _original;
        entry.original_filename = &entry.original[0] + filename_idx;
        entry.filename_bytes = filename_bytes;
    }
    return true;

    case ntfsdupe::cfgs::Mode::file_redirect: {

        std::wstring _original_upper(ntfsdupe::helpers::upper(_original));
        std::wstring _target_upper(ntfsdupe::helpers::upper(_target));
        if (_original_upper.compare(0, filename_idx, _target_upper, 0, filename_idx) != 0) { // path mismatch
            NTFSDUPE_DBG(L"  path mismatch");
            return false;
        }
        if (_original_upper.compare(_target_upper) == 0) { // self redirection
            NTFSDUPE_DBG(L"  self redirection");
            return false;
        }
        if (file_must_exist && !std::filesystem::exists(_target)) { // not a problem
            NTFSDUPE_DBG(L"  mode is redirect but target file not found");
            return true;
        }

        {
            const auto& _original_upper_ref = *global_entries_storage.insert(_original_upper).first;
            FileCfgEntry& entry = file_entries[_original_upper_ref];
            entry.mode = FileType::original;
            entry.original = _original;
            entry.original_filename = &entry.original[0] + filename_idx;
            entry.target = _target;
            entry.target_filename = &entry.target[0] + filename_idx;
            entry.filename_bytes = filename_bytes;
			entry.hook_times.mode = hook_times_cfg;
			entry.hook_times.hook_time_n = hook_time_n;
        }

        {
            const auto& _target_upper_ref = *global_entries_storage.insert(_target_upper).first;
            FileCfgEntry& entry = file_entries[_target_upper_ref];
            entry.mode = FileType::target;
            entry.original = _original;
            entry.original_filename = &entry.original[0] + filename_idx;
            entry.target = _target;
            entry.target_filename = &entry.target[0] + filename_idx;
            entry.filename_bytes = filename_bytes;
        }
    }
    return true;

    default: NTFSDUPE_DBG(L"  unknown file entry mode %i", (int)mode); return false; // unknown type
    }

    return false;
}


static void hide_module_in_peb(const std::wstring& original)
{
    auto hmodule = GetModuleHandleW(original.c_str());
    if (!hmodule) return;

    if (ntfsdupe::ntapis::peb_helper::remove_from_peb(hmodule)) {
        NTFSDUPE_DBG(L"  module '%s' is unlinked/removed from PEB", original.c_str());
    } else {
        NTFSDUPE_DBG(L"  failed to unlink/remove module '%s' from PEB", original.c_str());
    }
}

bool ntfsdupe::cfgs::add_entry_module(Mode mode, const std::wstring& original, const std::wstring& target)
{
    size_t bad_original_char = original.find_first_of(L"\\/");
    size_t bad_target_char = target.find_first_of(L"\\/");
    if (bad_original_char != std::wstring::npos || bad_target_char != std::wstring::npos) {
        NTFSDUPE_DBG(L"  modules must have filenames only, not paths");
        return false;
    }

    std::wstring _original_upper(ntfsdupe::helpers::upper(original));
    unsigned short filename_bytes = (unsigned short)(original.size() * sizeof(original[0]));
    NTFSDUPE_DBG(L"  module filename_bytes = %u", filename_bytes); // TODO

    switch (mode) {
    case ntfsdupe::cfgs::Mode::module_prevent_load:
    case ntfsdupe::cfgs::Mode::module_hide_handle: {
        // it's ok if this failed, not critical since the module might not be loaded yet
        // but has to be called before adding the entry, since the above function hide_module_in_peb() uses GetModuleHandleW()
        hide_module_in_peb(original);

        const auto& _original_upper_ref = *global_entries_storage.insert(_original_upper).first;
        ModuleCfgEntry &entry = module_entries[_original_upper_ref];

        if (mode == ntfsdupe::cfgs::Mode::module_prevent_load) entry.mode = ModuleType::prevent_load;
        else if (mode == ntfsdupe::cfgs::Mode::module_hide_handle) entry.mode = ModuleType::hide_handle;
            
        entry.original_filename = original;
        entry.filename_bytes = filename_bytes;
    }
    return true;

    case ntfsdupe::cfgs::Mode::module_redirect: {
        std::wstring _target_upper(ntfsdupe::helpers::upper(target));
        if (_original_upper.compare(_target_upper) == 0) { // self redirection
            NTFSDUPE_DBG(L"  self redirection");
            return false;
        }

        {
            const auto& _original_upper_ref = *global_entries_storage.insert(_original_upper).first;
            ModuleCfgEntry& entry = module_entries[_original_upper_ref];
            entry.mode = ModuleType::original;
            entry.original_filename = original;
            entry.target_filename = target;
            entry.filename_bytes = filename_bytes;
        }

        {
            const auto& _target_upper_ref = *global_entries_storage.insert(_target_upper).first;
            ModuleCfgEntry& entry = module_entries[_target_upper_ref];
            entry.mode = ModuleType::target;
            entry.original_filename = original;
            entry.target_filename = target;
            entry.filename_bytes = filename_bytes;
        }
    }
    return true;

    default: NTFSDUPE_DBG(L"  unknown module entry mode %i", (int)mode); return false; // unknown type
    }

    return false;
}

bool ntfsdupe::cfgs::add_entry(Mode mode, const std::wstring &original, const std::wstring &target, bool file_must_exist, HookTimesMode hook_times_cfg, int hook_time_n)
{
    NTFSDUPE_DBG(L"ntfsdupe::cfgs::add_entry() %i '%s' '%s'", (int)mode, original.c_str(), target.c_str());

    if (original.empty()) {
        NTFSDUPE_DBG(L"  original str is empty");
        return false;
    }

    if ((mode == Mode::file_redirect || mode == Mode::module_redirect) &&
        target.empty()) {
        NTFSDUPE_DBG(L"  mode is redirect but target str is empty");
        return false;
    }

    try {
        switch (mode) {
        case ntfsdupe::cfgs::Mode::file_hide:
        case ntfsdupe::cfgs::Mode::file_redirect:
            return add_entry_file(mode, original, target, file_must_exist, hook_times_cfg, hook_time_n);

        case ntfsdupe::cfgs::Mode::module_prevent_load:
        case ntfsdupe::cfgs::Mode::module_hide_handle: 
        case ntfsdupe::cfgs::Mode::module_redirect:
            return add_entry_module(mode, original, target);

        default: NTFSDUPE_DBG(L"  unknown entry mode %i", (int)mode); return false; // unknown type
        }

        return false;
    } catch (const std::exception &e) {
        NTFSDUPE_DBG(e.what());
        return false;
    }
}

bool ntfsdupe::cfgs::load_file(const wchar_t *file)
{
    NTFSDUPE_DBG(L"ntfsdupe::cfgs::load_file() '%s'", file);
    if (!file || !file[0]) {
        NTFSDUPE_DBG(L"  empty file");
        return false;
    }

    try {
        std::ifstream f(file);
        
        if (!f.is_open()) {
            NTFSDUPE_DBG(L" failed to open file");
            return false;
        }

        auto j = json::parse(f);
        f.close();

        size_t added_entries = 0;
        for (const auto &item: j.items()) {
            try {
                NTFSDUPE_DBG(L"  parsing new entry");
                std::wstring original(ntfsdupe::helpers::str_to_wstr(item.key()));
                NTFSDUPE_DBG(L"  original '%s'", original.c_str());

                Mode mode;
                std::string mode_str = item.value().value("mode", std::string());
                NTFSDUPE_DBG("  mode = " + mode_str);
                if (mode_str.compare("file_redirect") == 0) {
                    mode = Mode::file_redirect;
                } else if (mode_str.compare("file_hide") == 0) {
                    mode = Mode::file_hide;
                } else if (mode_str.compare("module_prevent_load") == 0) {
                    mode = Mode::module_prevent_load;
                } else if (mode_str.compare("module_redirect") == 0) {
                    mode = Mode::module_redirect;
                } else if (mode_str.compare("module_hide_handle") == 0) {
                    mode = Mode::module_hide_handle;
                } else {
                    NTFSDUPE_DBG(L"  invalid mode, skipping entry");
                    continue;
                }

                std::wstring target(
                    ntfsdupe::helpers::str_to_wstr(
                        item.value().value("to", std::string())
                    )
                );
                NTFSDUPE_DBG(L"  target '%s'", target.c_str());

                bool file_must_exist = item.value().value("file_must_exist", false);
                NTFSDUPE_DBG(L"  file_must_exist '%i'", (int)file_must_exist);

                HookTimesMode hook_times_cfg = HookTimesMode::all;
                std::string hook_times_str = item.value().value("hook_times_mode", std::string());
                if (hook_times_str.compare("all") == 0) {
                    hook_times_cfg = HookTimesMode::all;
                }
                else if (hook_times_str.compare("nth_time_only") == 0) {
                    hook_times_cfg = HookTimesMode::nth_time_only;
                }
                else if (hook_times_str.compare("not_nth_time_only") == 0) {
                    hook_times_cfg = HookTimesMode::not_nth_time_only;
                }
                NTFSDUPE_DBG(L"  hook_times_mode = %d", hook_times_cfg);

                int hook_time_n = item.value().value("hook_time_n", 0);
                NTFSDUPE_DBG(L"  hook_time_n: '%i'", hook_time_n);

                if (add_entry(mode, original, target, file_must_exist, hook_times_cfg, hook_time_n)) ++added_entries;
            } catch (const std::exception &e) {
                NTFSDUPE_DBG(e.what());
            }
        }

        return added_entries > 0;
    } catch (const std::exception &e) {
        NTFSDUPE_DBG(e.what());
        return false;
    }
}

ntfsdupe::cfgs::FileCfgEntry* ntfsdupe::cfgs::find_file_entry(const std::wstring_view &str) noexcept
{
    if (ntfsdupe::cfgs::file_entries.empty()) return nullptr;

    const auto &res = ntfsdupe::cfgs::file_entries.find(str);
    if (res == ntfsdupe::cfgs::file_entries.end()) {
        return nullptr;
    } else {
        return is_bypassed(str)
            ? nullptr
            : &res->second;
    }
}

const ntfsdupe::cfgs::ModuleCfgEntry* ntfsdupe::cfgs::find_module_entry(const std::wstring_view& str) noexcept
{
    if (ntfsdupe::cfgs::module_entries.empty()) return nullptr;

    const auto& res = ntfsdupe::cfgs::module_entries.find(str);
    return res == ntfsdupe::cfgs::module_entries.end()
        ? nullptr
        : &res->second;
}

void ntfsdupe::cfgs::add_bypass(const std::wstring_view &str) noexcept
{
    EnterCriticalSection(&bypass_files_cs);
    //NTFSDUPE_DBG(L"ntfsdupe::cfgs::add_bypass '%.*s'", (int)str.size(), str.data());
    ++bypass_entries[GetCurrentThreadId()][str];
    LeaveCriticalSection(&bypass_files_cs);
}

void ntfsdupe::cfgs::remove_bypass(const std::wstring_view &str) noexcept
{
    EnterCriticalSection(&bypass_files_cs);
    //NTFSDUPE_DBG(L"ntfsdupe::cfgs::remove_bypass '%.*s'", (int)str.size(), str.data());
    if (bypass_entries.size()) {
        DWORD tid = GetCurrentThreadId();
        auto it_tid = bypass_entries.find(tid);
        if (bypass_entries.end() != it_tid) { // thread ID exists
            auto &str_count_map = it_tid->second;
            auto it_str = str_count_map.find(str);
            if (str_count_map.end() != it_str) { // string exists
                if (it_str->second > 1) {
                    --it_str->second;
                } else { // about to remove the last string
                    str_count_map.erase(str);
                    if (!str_count_map.size()) {
                        bypass_entries.erase(tid);
                    }
                }
            }
        }
    }
    LeaveCriticalSection(&bypass_files_cs);
}

bool ntfsdupe::cfgs::is_bypassed(const std::wstring_view &str) noexcept
{
    bool ret = false;

    EnterCriticalSection(&bypass_files_cs);
    //NTFSDUPE_DBG(L"ntfsdupe::cfgs::is_bypassed '%.*s'", (int)str.size(), str.data());
    if (bypass_entries.size()) {
        auto tid_it = bypass_entries.find(GetCurrentThreadId());
        if (tid_it != bypass_entries.end()) {
            ret = tid_it->second.find(str) != tid_it->second.end();
        }
    }
    LeaveCriticalSection(&bypass_files_cs);
    
    return ret;
}
