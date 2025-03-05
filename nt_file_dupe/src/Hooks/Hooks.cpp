#include "Helpers/Helpers.hpp"
#include "Hooks/Hooks.hpp"
#include "NtApis/NtApis.hpp"

#include "Detours/detours.h"


void ntfsdupe::hooks::copy_new_file_target(wchar_t* object_name_new, const PUNICODE_STRING ObjectName, const ntfsdupe::cfgs::FileCfgEntry* cfg)
{
    const auto path_bytes = ObjectName->Length - cfg->filename_bytes;
    const auto new_filename_bytes = wcslen(cfg->target_filename) * sizeof(wchar_t);
    // copy original objectname (path only)
    memcpy(object_name_new, ObjectName->Buffer, path_bytes);
    // write target name
    memcpy((char*)object_name_new + path_bytes, cfg->target_filename, new_filename_bytes);

    object_name_new[(path_bytes + new_filename_bytes) / sizeof(wchar_t)] = L'\0';
}

void ntfsdupe::hooks::copy_new_module_target(wchar_t* dllname_new, const PUNICODE_STRING DllName, const ntfsdupe::cfgs::ModuleCfgEntry* cfg)
{
    const auto path_bytes = DllName->Length - cfg->filename_bytes;
    const auto new_filename_bytes = wcslen(cfg->target_filename.c_str()) * sizeof(wchar_t);
    // copy just the path
    memcpy(dllname_new, DllName->Buffer, path_bytes);
    // copy the new target filename
    memcpy((char*)dllname_new + path_bytes, cfg->target_filename.c_str(), new_filename_bytes);

    dllname_new[(path_bytes + new_filename_bytes) / sizeof(wchar_t)] = L'\0';
}


const ntfsdupe::cfgs::FileCfgEntry* ntfsdupe::hooks::find_single_file_obj_dospath(wchar_t *dos_path)
{
    if (!dos_path || !dos_path[0]) return nullptr;

    // first call to get required bytes including null
    USHORT fullDosPathBytes = 0;
    ntfsdupe::ntapis::GetFullDosPath(NULL, &fullDosPathBytes, dos_path);
    if (!fullDosPathBytes) return nullptr;

    auto fullDosPath = unique_ptr_stack(wchar_t, fullDosPathBytes);
    if (!fullDosPath) return nullptr;

    // convert dos path to full
    bool conversion = ntfsdupe::ntapis::GetFullDosPath(fullDosPath.get(), &fullDosPathBytes, dos_path);
    if (!conversion) return nullptr;

    ntfsdupe::helpers::upper(fullDosPath.get(), fullDosPathBytes / sizeof(wchar_t));
    return ntfsdupe::cfgs::find_file_entry(std::wstring_view(fullDosPath.get(), fullDosPathBytes / sizeof(wchar_t)));
}

const ntfsdupe::cfgs::FileCfgEntry* ntfsdupe::hooks::find_single_file_obj_ntpath(PCWSTR ntpath, ULONG ntpath_bytes)
{
    if (!ntpath || !ntpath[0] || !ntpath_bytes) return nullptr;

    USHORT dosPathBytes = (USHORT)(ntpath_bytes + sizeof(wchar_t));
    auto dosPath = unique_ptr_stack(wchar_t, dosPathBytes);
    if (!dosPath) return nullptr;

    // get dos path
    if (!ntfsdupe::ntapis::NtPathToDosPath(dosPath.get(), &dosPathBytes, ntpath, (USHORT)ntpath_bytes)) return nullptr;

    return find_single_file_obj_dospath(dosPath.get());
}

const ntfsdupe::cfgs::FileCfgEntry* ntfsdupe::hooks::find_single_file_obj_ntpath(POBJECT_ATTRIBUTES ObjectAttributes)
{
    if (!ObjectAttributes || !ObjectAttributes->ObjectName) return nullptr;

    return find_single_file_obj_ntpath(ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length);
}

const ntfsdupe::cfgs::FileCfgEntry* ntfsdupe::hooks::find_single_file_obj_base_handle(MultiQueryOffsets_t& query_info, HANDLE FileHandle, PVOID FileInformation)
{
    // get base path from handle
    IO_STATUS_BLOCK localIoStatus;
    auto basePath = null_unique_ptr_stack(FILE_NAME_INFORMATION);
    ULONG sz = sizeof(FILE_NAME_INFORMATION);
    NTSTATUS st = STATUS_BUFFER_OVERFLOW;
    do {
        sz += MAX_PATH;
        basePath = unique_ptr_stack(FILE_NAME_INFORMATION, sz);
        if (!basePath) return nullptr;

        st = NtQueryInformationFile_original(
            FileHandle,
            &localIoStatus,
            basePath.get(),
            sz,
            (FILE_INFORMATION_CLASS)FILE_INFORMATION_CLASS_ACTUAL::FileNormalizedNameInformation
        );
    } while (st == STATUS_BUFFER_OVERFLOW);
    if (!NT_SUCCESS(st)) return nullptr;

    // objects returned from query are not null terminated
    const auto node_filename_bytes = *(ULONG*)((char*)FileInformation + query_info.node_filename_bytes_offset);
    const size_t totalNameBytes = basePath->FileNameLength + sizeof(L'\\') + node_filename_bytes;
    auto nameTerminated = unique_ptr_stack(wchar_t, totalNameBytes + sizeof(wchar_t));
    if (!nameTerminated) return nullptr;

    // base path
    memcpy(nameTerminated.get(), basePath->FileName, basePath->FileNameLength);
    // '\'
    nameTerminated.get()[basePath->FileNameLength / sizeof(wchar_t)] = L'\\';
    // node filename
    memcpy(
        nameTerminated.get() + basePath->FileNameLength / sizeof(wchar_t) + 1,
        (char*)FileInformation + query_info.node_filename_offset, //FileInformation->FileName,
        *(ULONG*)((char*)FileInformation + query_info.node_filename_bytes_offset) //FileInformation->FileNameLength
    );
    // '\0'
    nameTerminated.get()[totalNameBytes / sizeof(wchar_t)] = L'\0';
    return ntfsdupe::hooks::find_single_file_obj_dospath(nameTerminated.get());
}

const ntfsdupe::cfgs::ModuleCfgEntry* ntfsdupe::hooks::find_module_obj_filename(PUNICODE_STRING  DllName)
{
    // find last '/' or '\'
    USHORT dllname_chars = DllName->Length / sizeof(wchar_t);
    int filename_idx = dllname_chars - 1;
    for (; filename_idx > 0; --filename_idx) {
        if (DllName->Buffer[filename_idx] == L'\\' || DllName->Buffer[filename_idx] == L'/') {
            ++filename_idx; // point at first char of the filename
            break;
        }
    }
    if (filename_idx < 0) filename_idx = 0;

    USHORT filename_bytes = DllName->Length - (USHORT)(filename_idx * sizeof(wchar_t));
    auto name_copy = unique_ptr_stack(wchar_t, filename_bytes);
    if (name_copy) {
        memcpy(name_copy.get(), DllName->Buffer + filename_idx, filename_bytes);
        USHORT filename_chars = dllname_chars - filename_idx;
        ntfsdupe::helpers::upper(name_copy.get(), filename_chars);
        return ntfsdupe::cfgs::find_module_entry(std::wstring_view(name_copy.get(), filename_chars));
    }

    return nullptr;
}

NTSTATUS ntfsdupe::hooks::handle_file_multi_query(
    MultiQueryOffsets_t& query_info,
    HANDLE FileHandle,
    PVOID FileInformation,
    PIO_STATUS_BLOCK IoStatusBlock,
    NTSTATUS result)
{
    // get base path from handle
    IO_STATUS_BLOCK localIoStatus;
    auto basePath = null_unique_ptr_stack(FILE_NAME_INFORMATION);
    ULONG sz = sizeof(FILE_NAME_INFORMATION);
    NTSTATUS st = STATUS_BUFFER_OVERFLOW;
    do {
        sz += MAX_PATH;
        basePath = unique_ptr_stack(FILE_NAME_INFORMATION, sz);
        if (!basePath) return result;

        st = NtQueryInformationFile_original(
            FileHandle,
            &localIoStatus,
            basePath.get(),
            sz,
            (FILE_INFORMATION_CLASS)FILE_INFORMATION_CLASS_ACTUAL::FileNormalizedNameInformation
        );

    } while (st == STATUS_BUFFER_OVERFLOW);
    if (!NT_SUCCESS(st)) return result;

    void* currentNode = FileInformation;
    ULONG currentNodeBytes = *(ULONG*)((char*)currentNode + query_info.node_next_entry_offset);

    void* previousNode = FileInformation;
    ULONG calculatedNodesBytes = 0;

    do {
        // objects returned from query are not null terminated
        const auto node_filename_bytes = *(ULONG*)((char*)currentNode + query_info.node_filename_bytes_offset);
        const size_t totalNameBytes = basePath->FileNameLength + sizeof(L'\\') + node_filename_bytes;
        auto nameTerminated = unique_ptr_stack(wchar_t, totalNameBytes + sizeof(wchar_t));
        if (!nameTerminated) return result;

        // base path
        memcpy(nameTerminated.get(), basePath->FileName, basePath->FileNameLength);
        // '\'
        nameTerminated.get()[basePath->FileNameLength / sizeof(wchar_t)] = L'\\';
        // node filename
        memcpy(
            nameTerminated.get() + basePath->FileNameLength / sizeof(wchar_t) + 1,
            (char*)currentNode + query_info.node_filename_offset, //FileInformation->FileName,
            node_filename_bytes //FileInformation->FileNameLength
        );
        // '\0'
        nameTerminated.get()[totalNameBytes / sizeof(wchar_t)] = L'\0';

        currentNodeBytes = *(ULONG*)((char*)currentNode + query_info.node_next_entry_offset);
        calculatedNodesBytes += currentNodeBytes;
        bool isNodeSkipped = false;

        const ntfsdupe::cfgs::FileCfgEntry* cfg = ntfsdupe::hooks::find_single_file_obj_dospath(nameTerminated.get());
        if (cfg) switch (cfg->mode) {
        // if this is an ignored file then skip it
        // also if original (redirected) file skip it because we should only report the target file (with fixed/restored name)
        case ntfsdupe::cfgs::FileType::original:
        case ntfsdupe::cfgs::FileType::hide: {
            NTFSDUPE_DBG(L"  multi query original/hide '%s'", cfg->original.c_str());
            // if no remaining nodes, this happens at the last node
            // or at the first node when it's the only remaining one
            if (!currentNodeBytes) {
                if (currentNode == FileInformation) { // if 1st node
                    IoStatusBlock->Information = 0;
                    return STATUS_OBJECT_NAME_NOT_FOUND;
                } else { // if last node
                    IoStatusBlock->Information = calculatedNodesBytes;
                    *(ULONG*)((char*)previousNode + query_info.node_next_entry_offset) = 0;
                    return result;
                }
            }
            else { // if there are still some upcoming nodes
                isNodeSkipped = true;
                // shift upcoming nodes to our location
                memcpy(
                    currentNode,
                    (char*)currentNode + currentNodeBytes,
                    IoStatusBlock->Information - calculatedNodesBytes
                );
                // subtract the ignored node size
                IoStatusBlock->Information -= currentNodeBytes;
                calculatedNodesBytes -= currentNodeBytes;
            }
        }
        break;

        case ntfsdupe::cfgs::FileType::target: { // restore the original name
            NTFSDUPE_DBG(L"  multi query target '%s'", cfg->target.c_str());
            memcpy(
                (char*)currentNode + query_info.node_filename_offset,
                cfg->original_filename,
                cfg->filename_bytes
            );
        }
        break;

        default: break;
        }

        // if current node was skipped (overwritten) then the pointers are already
        // pointing at the new node, no need to update
        if (!isNodeSkipped) {
            previousNode = currentNode;
            currentNode = (char*)currentNode + currentNodeBytes;
        }
    } while (currentNodeBytes);


    return result;
}


namespace ntfsdupe::hooks
{
    typedef struct
    {
        const char* const api_name;
        const LPVOID api_hook;
        LPVOID* const original_addr;
        const bool optional;
    } HookDescription_t;


    static HookDescription_t HooksDescriptions[] = {
        { "OpenFile", NtOpenFile_hook, (LPVOID*)&NtOpenFile_original, false },
        { "CreateFile", NtCreateFile_hook, (LPVOID*)&NtCreateFile_original, false },
        { "DeleteFile", NtDeleteFile_hook, (LPVOID*)&NtDeleteFile_original, false },
        // from docs: "[This function may be altered or unavailable in the future.]"
        { "OpenDirectoryObject", NtOpenDirectoryObject_hook, (LPVOID*)&NtOpenDirectoryObject_original, true },
        { "CreateDirectoryObject", NtCreateDirectoryObject_hook, (LPVOID*)&NtCreateDirectoryObject_original, false },
        // not found on Windows XP
        { "CreateDirectoryObjectEx", NtCreateDirectoryObjectEx_hook, (LPVOID*)&NtCreateDirectoryObjectEx_original, true },

        // ---------------------------------------- query ---------------------------------------- //
        // from docs: "[This function may be changed or removed from Windows without further notice.]"
        { "QueryAttributesFile", NtQueryAttributesFile_hook, (LPVOID*)&NtQueryAttributesFile_original, true },
        { "QueryFullAttributesFile", NtQueryFullAttributesFile_hook, (LPVOID*)&NtQueryFullAttributesFile_original, false },
        { "QueryInformationFile", NtQueryInformationFile_hook, (LPVOID*)&NtQueryInformationFile_original, false },
        // not found on Windows XP
        { "QueryInformationByName", NtQueryInformationByName_hook, (LPVOID*)&NtQueryInformationByName_original, true },
        { "QueryDirectoryFile", NtQueryDirectoryFile_hook, (LPVOID*)&NtQueryDirectoryFile_original, false },
        // minimum Windows 10, version 1709
        { "QueryDirectoryFileEx", NtQueryDirectoryFileEx_hook, (LPVOID*)&NtQueryDirectoryFileEx_original, false },

    };
}


bool ntfsdupe::hooks::init(void)
{
    NTFSDUPE_DBG(L"ntfsdupe::hooks::init()");
    auto NtdllHmod = GetModuleHandleW(L"ntdll.dll");
    if (!NtdllHmod) {
        NTFSDUPE_DBG(L"  failed to get ntdll.dll hmodule");
        return false;
    }

    if (DetourTransactionBegin() != NO_ERROR) return false;
    if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) return false;

    for (auto &hook_desc : HooksDescriptions) {
        *hook_desc.original_addr = GetProcAddress(NtdllHmod, (std::string("Nt") + hook_desc.api_name).c_str());
        if (!*hook_desc.original_addr) *hook_desc.original_addr = GetProcAddress(NtdllHmod, (std::string("Zw") + hook_desc.api_name).c_str());

        if (!*hook_desc.original_addr ||
            (DetourAttach(hook_desc.original_addr, hook_desc.api_hook) != NO_ERROR)) {
            if (!hook_desc.optional) {
                NTFSDUPE_DBG(std::string("  failed to hook Nt") + hook_desc.api_name + "()");
                return false;
            }
        }

    }

    LdrLoadDll_original = (decltype(LdrLoadDll_original))GetProcAddress(NtdllHmod, "LdrLoadDll");
    if (!LdrLoadDll_original ||
        (DetourAttach(&LdrLoadDll_original, LdrLoadDll_hook) != NO_ERROR)) {
            NTFSDUPE_DBG(L"  failed to hook LdrLoadDll()");
            return false;
    }
    
    LdrGetDllHandle_original = (decltype(LdrGetDllHandle_original))GetProcAddress(NtdllHmod, "LdrGetDllHandle");
    if (!LdrGetDllHandle_original ||
        (DetourAttach(&LdrGetDllHandle_original, LdrGetDllHandle_hook) != NO_ERROR)) {
            NTFSDUPE_DBG(L"  failed to hook LdrGetDllHandle()");
            return false;
    }

    return DetourTransactionCommit() == NO_ERROR;
}


void ntfsdupe::hooks::deinit(void)
{
    NTFSDUPE_DBG(L"ntfsdupe::hooks::deinit()");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (const auto &h : HooksDescriptions) {
        if (*h.original_addr) DetourDetach(h.original_addr, h.api_hook);
    }

    DetourDetach(&LdrLoadDll_original, LdrLoadDll_hook);
    DetourDetach(&LdrGetDllHandle_original, LdrGetDllHandle_hook);
    DetourTransactionCommit();
}
