#pragma once
#include <windows.h>
#include "injector/injector.hpp"

namespace Plugin
{
	void Init(HMODULE);

	enum class eGameVersion
	{
		UNCHECKED,
		IV_1_0_8_0,
		EFLC_1_1_3_0,
		UNKNOWN
	};

	eGameVersion GetGameVersion();

	injector::auto_pointer AddressByVersion(unsigned int addressiv, unsigned int addresseflc = 0);
};
