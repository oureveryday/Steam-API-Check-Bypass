#include "nt_file_dupe.hpp"

#include "lib_main/lib_main.hpp"
#include "Configs/Configs.hpp"
#include "Helpers/Helpers.hpp"
#include "Hooks/Hooks.hpp"

#include <string>
#include <filesystem>


bool NTFSDUPE_DECL ntfsdupe_add_entry(
	ntfsdupe::itf::Mode mode,
	const wchar_t *original,
	const wchar_t *target,
	bool file_must_exist
)
{
	if (!original || !original[0]) return false;
	
	std::wstring _target = target && target[0]
		? std::wstring(target)
		: std::wstring();
	return ntfsdupe::cfgs::add_entry((ntfsdupe::cfgs::Mode)mode, original, _target, file_must_exist);
}

bool NTFSDUPE_DECL ntfsdupe_load_file(const wchar_t *file)
{
	return ntfsdupe::cfgs::load_file(file);
}

void NTFSDUPE_DECL ntfsdupe_deinit()
{
	ntfsdupe::deinit();
}
