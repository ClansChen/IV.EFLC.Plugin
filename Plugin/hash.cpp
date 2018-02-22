#include "hash.h"
#include "plugin.h"

namespace Hash
{
	std::uint32_t HashStringFromSeediCase(const char *str, std::uint32_t seed)
	{
		return injector::cstd<std::uint32_t(const char *, std::uint32_t)>::call(Plugin::AddressByVersion(0x45CF50), str, seed);
	}
}
