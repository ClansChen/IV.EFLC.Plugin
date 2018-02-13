#include "table.h"
#include <vector>
#include <memory>
#include <fstream>

static uint8_t table[0x20000];

void Table::LoadTable(const char *filename)
{
	std::ifstream input(filename, std::ios_base::binary);

	if (input)
	{
		input.read(reinterpret_cast<char *>(table), 0x20000);
	}
}

const CharPos &Table::GetCharPos(wchar_t character)
{
	return table[character];
}
