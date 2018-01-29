#include "uint32_hex.h"

bool uint32_hex::digit_to_value(char digit, std::uint8_t &result)
{
	switch (digit)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		result = (digit - '0');
		return true;

	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		result = (digit - 'A' + 0xA);
		return true;

	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		result = (digit - 'a' + 0xA);
		return true;

	default:
		return false;
	}
}

bool uint32_hex::value_to_digit(std::uint8_t value, char &result)
{
	if (value < 10)
	{
		result = ('0' + value);
		return true;
	}
	else if (value < 16)
	{
		result = ('A' + value - 0xA);
		return true;
	}
	else
	{
		return false;
	}
}

bool uint32_hex::from_hex(const std::array<char, 11> &hex, std::uint32_t &result)
{
	std::uint8_t values[8];

	__helper helper;

	if (hex[0] == '0' &&
		(hex[1] == 'x' || hex[1] == 'X') &&
		hex[10] == 0 &&
		digit_to_value(hex[2], values[7]) &&
		digit_to_value(hex[3], values[6]) &&
		digit_to_value(hex[4], values[5]) &&
		digit_to_value(hex[5], values[4]) &&
		digit_to_value(hex[6], values[3]) &&
		digit_to_value(hex[7], values[2]) &&
		digit_to_value(hex[8], values[1]) &&
		digit_to_value(hex[9], values[0]))
	{
		helper.splited.bits_0_3 = values[0];
		helper.splited.bits_4_7 = values[1];
		helper.splited.bits_8_11 = values[2];
		helper.splited.bits_12_15 = values[3];
		helper.splited.bits_16_19 = values[4];
		helper.splited.bits_20_23 = values[5];
		helper.splited.bits_24_27 = values[6];
		helper.splited.bits_28_31 = values[7];

		result = helper.whole;
		return true;
	}
	else
	{
		return false;
	}

}

void uint32_hex::to_hex(std::uint32_t value, std::array<char, 11> &result)
{
	std::array<char, 11> temp;
	__helper helper;

	temp[0] = '0';
	temp[1] = 'x';
	temp[10] = 0;

	helper.whole = value;

	value_to_digit(helper.splited.bits_0_3, temp[9]);
	value_to_digit(helper.splited.bits_4_7, temp[8]);
	value_to_digit(helper.splited.bits_8_11, temp[7]);
	value_to_digit(helper.splited.bits_12_15, temp[6]);
	value_to_digit(helper.splited.bits_16_19, temp[5]);
	value_to_digit(helper.splited.bits_20_23, temp[4]);
	value_to_digit(helper.splited.bits_24_27, temp[3]);
	value_to_digit(helper.splited.bits_28_31, temp[2]);
	
	result = temp;
}
