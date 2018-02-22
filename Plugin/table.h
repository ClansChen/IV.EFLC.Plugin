#pragma once
#include "stdinc.h"

namespace Table
{
	void LoadTable(const std::experimental::filesystem::v1::path &);

	std::uint8_t GetCharRow(std::uint16_t);
	std::uint8_t GetCharColumn(std::uint16_t);
};

