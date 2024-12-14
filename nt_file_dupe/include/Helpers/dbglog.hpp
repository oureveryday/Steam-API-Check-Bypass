#pragma once

#if !defined(NT_FS_DUPE_RELEASE)

#include <string>

namespace ntfsdupe::helpers::dbglog {

    bool init();

    void write(const wchar_t *fmt, ...);
    void write(const std::wstring &str);
    void write(const std::string &str);

    void close();

}

#define NTFSDUPE_DBG_INIT() ntfsdupe::helpers::dbglog::init()
#define NTFSDUPE_DBG_CLOSE() ntfsdupe::helpers::dbglog::close()
#define NTFSDUPE_DBG(fmt, ...) ntfsdupe::helpers::dbglog::write(fmt, __VA_ARGS__)

#else

#define NTFSDUPE_DBG_INIT()
#define NTFSDUPE_DBG_CLOSE()
#define NTFSDUPE_DBG(...)

#endif // NT_FS_DUPE_RELEASE
