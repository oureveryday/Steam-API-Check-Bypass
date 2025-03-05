#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winternl.h>

#if !defined(STATUS_OBJECT_NAME_NOT_FOUND)
    #define STATUS_OBJECT_NAME_NOT_FOUND 0xC0000034
#endif

#if !defined(STATUS_BUFFER_OVERFLOW)
    #define STATUS_BUFFER_OVERFLOW 0x80000005
#endif

#include "NtApis/NtApis.hpp"
#include "Configs/Configs.hpp"
#include "Helpers/Helpers.hpp"
#include "Helpers/dbglog.hpp"

#include "Hooks/NtOpenFile_hook/NtOpenFile_hook.hpp"
#include "Hooks/NtCreateFile_hook/NtCreateFile_hook.hpp"
#include "Hooks/NtDeleteFile_hook/NtDeleteFile_hook.hpp"
#include "Hooks/NtOpenDirectoryObject_hook/NtOpenDirectoryObject_hook.hpp"
#include "Hooks/NtCreateDirectoryObject_hook/NtCreateDirectoryObject_hook.hpp"
#include "Hooks/NtCreateDirectoryObjectEx_hook/NtCreateDirectoryObjectEx_hook.hpp"
#include "Hooks/NtQueryAttributesFile_hook/NtQueryAttributesFile_hook.hpp"
#include "Hooks/NtQueryFullAttributesFile_hook/NtQueryFullAttributesFile_hook.hpp"
#include "Hooks/NtQueryInformationFile_hook/NtQueryInformationFile_hook.hpp"

#include "Hooks/NtQueryInformationByName_hook/NtQueryInformationByName_hook.hpp"

#include "Hooks/NtQueryDirectoryFile_hook/NtQueryDirectoryFile_hook.hpp"
#include "Hooks/NtQueryDirectoryFileEx_hook/NtQueryDirectoryFileEx_hook.hpp"

#include "Hooks/LdrLoadDll_hook/LdrLoadDll_hook.hpp"
#include "Hooks/LdrGetDllHandle_hook/LdrGetDllHandle_hook.hpp"


namespace ntfsdupe::hooks {
    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryinformationfile
    // the one that comes with Visual Studio has only 1 entry
    typedef enum {
        FileDirectoryInformation = 1,
        FileFullDirectoryInformation,                   // 2
        FileBothDirectoryInformation,                   // 3
        FileBasicInformation,                           // 4
        FileStandardInformation,                        // 5
        FileInternalInformation,                        // 6
        FileEaInformation,                              // 7
        FileAccessInformation,                          // 8
        FileNameInformation,                            // 9
        FileRenameInformation,                          // 10
        FileLinkInformation,                            // 11
        FileNamesInformation,                           // 12
        FileDispositionInformation,                     // 13
        FilePositionInformation,                        // 14
        FileFullEaInformation,                          // 15
        FileModeInformation,                            // 16
        FileAlignmentInformation,                       // 17
        FileAllInformation,                             // 18
        FileAllocationInformation,                      // 19
        FileEndOfFileInformation,                       // 20
        FileAlternateNameInformation,                   // 21
        FileStreamInformation,                          // 22
        FilePipeInformation,                            // 23
        FilePipeLocalInformation,                       // 24
        FilePipeRemoteInformation,                      // 25
        FileMailslotQueryInformation,                   // 26
        FileMailslotSetInformation,                     // 27
        FileCompressionInformation,                     // 28
        FileObjectIdInformation,                        // 29
        FileCompletionInformation,                      // 30
        FileMoveClusterInformation,                     // 31
        FileQuotaInformation,                           // 32
        FileReparsePointInformation,                    // 33
        FileNetworkOpenInformation,                     // 34
        FileAttributeTagInformation,                    // 35
        FileTrackingInformation,                        // 36
        FileIdBothDirectoryInformation,                 // 37
        FileIdFullDirectoryInformation,                 // 38
        FileValidDataLengthInformation,                 // 39
        FileShortNameInformation,                       // 40
        FileIoCompletionNotificationInformation,        // 41
        FileIoStatusBlockRangeInformation,              // 42
        FileIoPriorityHintInformation,                  // 43
        FileSfioReserveInformation,                     // 44
        FileSfioVolumeInformation,                      // 45
        FileHardLinkInformation,                        // 46
        FileProcessIdsUsingFileInformation,             // 47
        FileNormalizedNameInformation,                  // 48
        FileNetworkPhysicalNameInformation,             // 49
        FileIdGlobalTxDirectoryInformation,             // 50
        FileIsRemoteDeviceInformation,                  // 51
        FileUnusedInformation,                          // 52
        FileNumaNodeInformation,                        // 53
        FileStandardLinkInformation,                    // 54
        FileRemoteProtocolInformation,                  // 55

        //
        //  These are special versions of these operations (defined earlier)
        //  which can be used by kernel mode drivers only to bypass security
        //  access checks for Rename and HardLink operations.  These operations
        //  are only recognized by the IOManager, a file system should never
        //  receive these.
        //

        FileRenameInformationBypassAccessCheck,         // 56
        FileLinkInformationBypassAccessCheck,           // 57

        //
        // End of special information classes reserved for IOManager.
        //

        FileVolumeNameInformation,                      // 58
        FileIdInformation,                              // 59
        FileIdExtdDirectoryInformation,                 // 60
        FileReplaceCompletionInformation,               // 61
        FileHardLinkFullIdInformation,                  // 62
        FileIdExtdBothDirectoryInformation,             // 63
        FileDispositionInformationEx,                   // 64
        FileRenameInformationEx,                        // 65
        FileRenameInformationExBypassAccessCheck,       // 66
        FileDesiredStorageClassInformation,             // 67
        FileStatInformation,                            // 68
        FileMemoryPartitionInformation,                 // 69
        FileStatLxInformation,                          // 70
        FileCaseSensitiveInformation,                   // 71
        FileLinkInformationEx,                          // 72
        FileLinkInformationExBypassAccessCheck,         // 73
        FileStorageReserveIdInformation,                // 74
        FileCaseSensitiveInformationForceAccessCheck,   // 75
        FileKnownFolderInformation,                     // 76

        FileMaximumInformation
    } FILE_INFORMATION_CLASS_ACTUAL, * PFILE_INFORMATION_CLASS_ACTUAL;
}


namespace ntfsdupe::hooks {
    typedef struct _FILE_NAME_INFORMATION {
        ULONG FileNameLength;
        WCHAR FileName[1];
    } FILE_NAME_INFORMATION, * PFILE_NAME_INFORMATION;

    typedef struct _FILE_STANDARD_INFORMATION {
        LARGE_INTEGER AllocationSize;
        LARGE_INTEGER EndOfFile;
        ULONG         NumberOfLinks;
        BOOLEAN       DeletePending;
        BOOLEAN       Directory;
    } FILE_STANDARD_INFORMATION, * PFILE_STANDARD_INFORMATION;

    typedef struct _FILE_INTERNAL_INFORMATION {
        LARGE_INTEGER IndexNumber;
    } FILE_INTERNAL_INFORMATION, * PFILE_INTERNAL_INFORMATION;

    typedef struct _FILE_EA_INFORMATION {
        ULONG EaSize;
    } FILE_EA_INFORMATION, * PFILE_EA_INFORMATION;

    typedef struct _FILE_ACCESS_INFORMATION {
        ACCESS_MASK AccessFlags;
    } FILE_ACCESS_INFORMATION, * PFILE_ACCESS_INFORMATION;

    typedef struct _FILE_POSITION_INFORMATION {
        LARGE_INTEGER CurrentByteOffset;
    } FILE_POSITION_INFORMATION, * PFILE_POSITION_INFORMATION;

    typedef struct _FILE_MODE_INFORMATION {
        ULONG Mode;
    } FILE_MODE_INFORMATION, * PFILE_MODE_INFORMATION;

    typedef struct _FILE_ALIGNMENT_INFORMATION {
        ULONG AlignmentRequirement;
    } FILE_ALIGNMENT_INFORMATION, * PFILE_ALIGNMENT_INFORMATION;

    typedef struct _FILE_ALL_INFORMATION {
        FILE_BASIC_INFORMATION     BasicInformation;
        FILE_STANDARD_INFORMATION  StandardInformation;
        FILE_INTERNAL_INFORMATION  InternalInformation;
        FILE_EA_INFORMATION        EaInformation;
        FILE_ACCESS_INFORMATION    AccessInformation;
        FILE_POSITION_INFORMATION  PositionInformation;
        FILE_MODE_INFORMATION      ModeInformation;
        FILE_ALIGNMENT_INFORMATION AlignmentInformation;
        FILE_NAME_INFORMATION      NameInformation;
    } FILE_ALL_INFORMATION, * PFILE_ALL_INFORMATION;


    // --- query types --- //
    typedef struct _FILE_DIRECTORY_INFORMATION {
        ULONG         NextEntryOffset;
        ULONG         FileIndex;
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER EndOfFile;
        LARGE_INTEGER AllocationSize;
        ULONG         FileAttributes;
        ULONG         FileNameLength;
        WCHAR         FileName[1];
    } FILE_DIRECTORY_INFORMATION, * PFILE_DIRECTORY_INFORMATION;

    typedef struct _FILE_FULL_DIR_INFORMATION {
        ULONG         NextEntryOffset;
        ULONG         FileIndex;
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER EndOfFile;
        LARGE_INTEGER AllocationSize;
        ULONG         FileAttributes;
        ULONG         FileNameLength;
        ULONG         EaSize;
        WCHAR         FileName[1];
    } FILE_FULL_DIR_INFORMATION, * PFILE_FULL_DIR_INFORMATION;

    typedef struct _FILE_BOTH_DIR_INFORMATION {
        ULONG         NextEntryOffset;
        ULONG         FileIndex;
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER EndOfFile;
        LARGE_INTEGER AllocationSize;
        ULONG         FileAttributes;
        ULONG         FileNameLength;
        ULONG         EaSize;
        CCHAR         ShortNameLength;
        WCHAR         ShortName[12];
        WCHAR         FileName[1];
    } FILE_BOTH_DIR_INFORMATION, * PFILE_BOTH_DIR_INFORMATION;

    typedef struct _FILE_NAMES_INFORMATION {
        ULONG NextEntryOffset;
        ULONG FileIndex;
        ULONG FileNameLength;
        WCHAR FileName[1];
    } FILE_NAMES_INFORMATION, * PFILE_NAMES_INFORMATION;

    typedef struct _FILE_ID_BOTH_DIR_INFORMATION {
        ULONG         NextEntryOffset;
        ULONG         FileIndex;
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER EndOfFile;
        LARGE_INTEGER AllocationSize;
        ULONG         FileAttributes;
        ULONG         FileNameLength;
        ULONG         EaSize;
        CCHAR         ShortNameLength;
        WCHAR         ShortName[12];
        LARGE_INTEGER FileId;
        WCHAR         FileName[1];
    } FILE_ID_BOTH_DIR_INFORMATION, * PFILE_ID_BOTH_DIR_INFORMATION;

    typedef struct _FILE_ID_FULL_DIR_INFORMATION {
        ULONG         NextEntryOffset;
        ULONG         FileIndex;
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER EndOfFile;
        LARGE_INTEGER AllocationSize;
        ULONG         FileAttributes;
        ULONG         FileNameLength;
        ULONG         EaSize;
        LARGE_INTEGER FileId;
        WCHAR         FileName[1];
    } FILE_ID_FULL_DIR_INFORMATION, * PFILE_ID_FULL_DIR_INFORMATION;

    typedef struct _FILE_ID_GLOBAL_TX_DIR_INFORMATION {
        ULONG         NextEntryOffset;
        ULONG         FileIndex;
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER EndOfFile;
        LARGE_INTEGER AllocationSize;
        ULONG         FileAttributes;
        ULONG         FileNameLength;
        LARGE_INTEGER FileId;
        GUID          LockingTransactionId;
        ULONG         TxInfoFlags;
        WCHAR         FileName[1];
    } FILE_ID_GLOBAL_TX_DIR_INFORMATION, * PFILE_ID_GLOBAL_TX_DIR_INFORMATION;

    // defined on win 10
    typedef struct _FILE_ID_128 {
        BYTE Identifier[16];
    } FILE_ID_128, * PFILE_ID_128;

    typedef struct _FILE_ID_EXTD_DIR_INFORMATION {
        ULONG         NextEntryOffset;
        ULONG         FileIndex;
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER EndOfFile;
        LARGE_INTEGER AllocationSize;
        ULONG         FileAttributes;
        ULONG         FileNameLength;
        ULONG         EaSize;
        ULONG         ReparsePointTag;
        FILE_ID_128   FileId;
        WCHAR         FileName[1];
    } FILE_ID_EXTD_DIR_INFORMATION, * PFILE_ID_EXTD_DIR_INFORMATION;

    typedef struct _FILE_ID_EXTD_BOTH_DIR_INFORMATION {
        ULONG         NextEntryOffset;
        ULONG         FileIndex;
        LARGE_INTEGER CreationTime;
        LARGE_INTEGER LastAccessTime;
        LARGE_INTEGER LastWriteTime;
        LARGE_INTEGER ChangeTime;
        LARGE_INTEGER EndOfFile;
        LARGE_INTEGER AllocationSize;
        ULONG         FileAttributes;
        ULONG         FileNameLength;
        ULONG         EaSize;
        ULONG         ReparsePointTag;
        FILE_ID_128   FileId;
        CCHAR         ShortNameLength;
        WCHAR         ShortName[12];
        WCHAR         FileName[1];
    } FILE_ID_EXTD_BOTH_DIR_INFORMATION, * PFILE_ID_EXTD_BOTH_DIR_INFORMATION;
}


namespace ntfsdupe::hooks {
    typedef struct {
        unsigned long node_next_entry_offset;
        unsigned long node_filename_offset;
        unsigned long node_filename_bytes_offset;
    } MultiQueryOffsets_t;
}

namespace ntfsdupe::hooks {
    void copy_new_file_target(wchar_t *object_name_new, const PUNICODE_STRING ObjectName, const ntfsdupe::cfgs::FileCfgEntry *cfg);
    void copy_new_module_target(wchar_t *dllname_new, const PUNICODE_STRING DllName, const ntfsdupe::cfgs::ModuleCfgEntry *cfg);

    const ntfsdupe::cfgs::FileCfgEntry* find_single_file_obj_dospath(wchar_t* dos_path); // dos path has to be null terminated
    const ntfsdupe::cfgs::FileCfgEntry* find_single_file_obj_ntpath(PCWSTR ntpath, ULONG ntpath_bytes);
    const ntfsdupe::cfgs::FileCfgEntry* find_single_file_obj_ntpath(POBJECT_ATTRIBUTES ObjectAttributes);
    const ntfsdupe::cfgs::FileCfgEntry* find_single_file_obj_base_handle(MultiQueryOffsets_t& query_info, HANDLE FileHandle, PVOID FileInformation);
    
    const ntfsdupe::cfgs::ModuleCfgEntry* find_module_obj_filename(PUNICODE_STRING  DllName);

    NTSTATUS handle_file_multi_query(
        MultiQueryOffsets_t& query_info,
        HANDLE FileHandle,
        PVOID FileInformation,
        PIO_STATUS_BLOCK IoStatusBlock,
        NTSTATUS result
    );
    
    bool init(void);
    
    void deinit(void);

}
