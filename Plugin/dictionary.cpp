#include "plugin.h"
#include "game.h"
#include "deps/injector/calling.hpp"

namespace Dictionary
{
	void *GetElementByKey(void *pDictionary, unsigned int hash)
	{
		return injector::thiscall<void *(void *, unsigned int)>::call(Plugin::AddressByVersion(0x43A490), pDictionary, hash);
	}
}
