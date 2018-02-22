#include "table.h"

static std::uint8_t table[0x20000];

void Table::LoadTable(const std::experimental::filesystem::v1::path &filename)
{
	std::ifstream input(filename, std::ios_base::binary);

	for (std::size_t index = 0; index < 0x10000; ++index)
	{
		table[index * 2] = 50;
		table[index * 2 + 1] = 63;
	}

	if (input)
	{
		input.read(reinterpret_cast<char *>(table), 0x20000);
	}
}

std::uint8_t Table::GetCharRow(std::uint16_t character)
{
	return table[character * 2];
}

std::uint8_t Table::GetCharColumn(std::uint16_t character)
{
	return table[character * 2 + 1];
}
