#include "lib_main/lib_main.hpp"
#include "Helpers/dbglog.hpp"
#include "Configs/Configs.hpp"
#include "NtApis/NtApis.hpp"
#include "Hooks/Hooks.hpp"

namespace ntfsdupe {
	struct CppRt {
		~CppRt() {
			NTFSDUPE_DBG(L"ntfsdupe::CppRt::~CppRt()");
			ntfsdupe::deinit();
		}
	};

	static CppRt cpp_rt{};
}

bool ntfsdupe::init()
{
	NTFSDUPE_DBG_INIT();

	if (!ntfsdupe::cfgs::init()) return false;
	if (!ntfsdupe::ntapis::init()) return false;
	if (!ntfsdupe::hooks::init()) return false;

	// hide the log file
#if !defined(NT_FS_DUPE_RELEASE)
	ntfsdupe::cfgs::add_entry(ntfsdupe::cfgs::Mode::file_hide, ntfsdupe::helpers::get_module_fullpath(nullptr) + L".NT_FS_DUPE.log");
#endif

	return true;
}

void ntfsdupe::deinit()
{
	ntfsdupe::hooks::deinit();
	ntfsdupe::cfgs::deinit();

	NTFSDUPE_DBG_CLOSE();
}
