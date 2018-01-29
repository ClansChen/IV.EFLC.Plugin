#pragma once

struct CharPos
{
	unsigned char row, column;
};

namespace Table
{
	void LoadTable(const char *);
	const CharPos &GetCharPos(wchar_t);
};

