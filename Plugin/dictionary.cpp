#include "dictionary.h"
#include "plugin.h"

namespace Dictionary
{
	void *GetElementByKey(void *pDictionary, std::uint32_t hash)
	{
		return injector::thiscall<void *(void *, std::uint32_t)>::call(Plugin::AddressByVersion(0x43A490), pDictionary, hash);
	}
}
