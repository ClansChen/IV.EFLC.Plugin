#include "table.h"
#include <vector>
#include <memory>
#include <fstream>

static std::vector<CharPos> table;

void Table::LoadTable(const char *filename)
{
	std::ifstream input(filename, std::ios_base::binary);

	table.clear();
	table.resize(0x10000, CharPos{ 50,63 });

	if (input)
	{
		input.read(reinterpret_cast<char *>(table.data()), 0x20000);
	}
}

const CharPos &Table::GetCharPos(wchar_t character)
{
	return table[character];
}
