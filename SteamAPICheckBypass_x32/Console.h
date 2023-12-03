#ifndef CONSOLE_H
#define CONSOLE_H

#include <Windows.h>
#include <stdio.h>

namespace Console
{
	void Attach();
	void Detach();
	bool Print(const char* fmt, ...);
}

#endif