#pragma once

/* 'borrowed' from headhunter */

#include <string>

void CreateConsole(const char* Name);

class output {
public:

	enum class color
	{
		red,
		green,
		blue,
		cyan,
		white,
		pink
	};

	output(std::string title);

	void close();
	void clear();

	static void printf(const char* s, ...);

	const output& operator<<(const std::string& str) const;
	const output& operator<<(color col) const;
};