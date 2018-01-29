#pragma once
#include <cstdint>
#include <array>

class uint32_hex
{
	union __helper
	{
		std::uint32_t whole;

		struct
		{
			std::uint8_t bits_0_3 : 4;
			std::uint8_t bits_4_7 : 4;
			std::uint8_t bits_8_11 : 4;
			std::uint8_t bits_12_15 : 4;
			std::uint8_t bits_16_19 : 4;
			std::uint8_t bits_20_23 : 4;
			std::uint8_t bits_24_27 : 4;
			std::uint8_t bits_28_31 : 4;
		} splited;
	};

	static bool digit_to_value(char digit, std::uint8_t &result);
	static bool value_to_digit(std::uint8_t value, char &result);

public:
	static bool from_hex(const std::array<char, 11> &hex, std::uint32_t &result);
	static void to_hex(std::uint32_t value, std::array<char, 11> &result);
};
