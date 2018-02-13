#include "plugin.h"
#include "game.h"
#include "injector/calling.hpp"

namespace Hash
{
	unsigned int HashStringFromSeediCase(const char *str, unsigned int seed)
	{
		return injector::cstd<unsigned int(const char *, unsigned int)>::call(Plugin::AddressByVersion(0x45CF50), str, seed);
	}
}
