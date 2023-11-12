#include "pch.h"
#include "Console.h"

HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

void Console::Attach()
{
    _old_out = GetStdHandle(STD_OUTPUT_HANDLE);
    _old_err = GetStdHandle(STD_ERROR_HANDLE);
    _old_in = GetStdHandle(STD_INPUT_HANDLE);

    ::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

    _out = GetStdHandle(STD_OUTPUT_HANDLE);
    _err = GetStdHandle(STD_ERROR_HANDLE);
    _in = GetStdHandle(STD_INPUT_HANDLE);

    SetConsoleMode(_out,
        ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

    SetConsoleMode(_in,
        ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
        ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
}

void Console::Detach()
{
    if (_out && _err && _in) {
        FreeConsole();

        if (_old_out)
            SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
        if (_old_err)
            SetStdHandle(STD_ERROR_HANDLE, _old_err);
        if (_old_in)
            SetStdHandle(STD_INPUT_HANDLE, _old_in);
    }
}

bool Console::Print(const char* fmt, ...)
{
    if (!_out)
        return false;

    char buf[1024];
    va_list va;

    va_start(va, fmt);
    _vsnprintf_s(buf, 1024, fmt, va);
    va_end(va);

    return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
}