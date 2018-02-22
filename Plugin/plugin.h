#pragma once
#include "stdinc.h"

namespace Plugin
{
	enum class eGameVersion
	{
		UNCHECKED,
		IV_1_0_8_0,
		EFLC_1_1_3_0,
		UNKNOWN
	};

	void Init(HMODULE);

	eGameVersion GetGameVersion();

	injector::auto_pointer AddressByVersion(std::uintptr_t addressiv, std::uintptr_t addresseflc = 0);
};
