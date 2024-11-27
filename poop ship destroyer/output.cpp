#include "output.h"

#include <Windows.h>
#include <iostream>

void CreateConsole(const char* Name) {
	DWORD thing;
	VirtualProtect((PVOID)&FreeConsole, 1, PAGE_EXECUTE_READWRITE, &thing);

	*(BYTE*)(&FreeConsole) = 0xC3;
	AllocConsole();

	SetConsoleTitleA(Name);

	freopen("CONIN$", "r", stdin);

	HWND cons = GetConsoleWindow();
	HMENU h = GetSystemMenu(cons, FALSE);

	EnableMenuItem(h, SC_CLOSE, MF_GRAYED);

	::ShowWindow(cons, SW_NORMAL); 
	
}

void output::close()
{
	HWND cons = GetConsoleWindow();

	::FreeConsole();

	::CloseWindow(cons);
	::ShowWindow(cons, SW_HIDE);
}

void output::printf(const char* s, ...)
{
	char cb[256];
	std::string b;
	va_list va;
	FILE* conout;

	va_start(va, s);
	vsprintf(cb, s, va);
	b = std::string(cb);
	va_end(va);

	conout = fopen("CONOUT$", "w");
	fwrite(b.c_str(), sizeof(char), b.size(), conout);
	fclose(conout);
}

void output::clear()
{
	system("cls");
}

output::output(std::string title)
{
	CreateConsole(title.c_str());
}

const output& output::operator<<(const std::string& str) const
{
	std::cout << str;
	return *this;
}

const output& output::operator<<(color col) const
{
	HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

	switch (col)
	{
	case color::red:
		SetConsoleTextAttribute(h_console, 12);
		break;
	case color::green:
		SetConsoleTextAttribute(h_console, 10);
		break;
	case color::blue:
		SetConsoleTextAttribute(h_console, 9);
		break;
	case color::pink:
		SetConsoleTextAttribute(h_console, 13);
		break;
	case color::cyan:
		SetConsoleTextAttribute(h_console, 11);
		break;
	case color::white:
		[[fallthrough]];
	default:
		SetConsoleTextAttribute(h_console, 7);
		break;
	}

	return *this;
}