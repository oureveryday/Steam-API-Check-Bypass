#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ntfsdupe::ntapis::peb_helper {
	bool remove_from_peb(HMODULE hModule);
}
