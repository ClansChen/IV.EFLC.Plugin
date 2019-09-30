#include "stdinc.h"
#include "dictionary.h"
#include "game.h"

namespace Dictionary
{
    void *GetElementByKey(void *pDictionary, std::uint32_t hash)
    {
        return injector::thiscall<void *(void *, std::uint32_t)>::call(Game::AddressByVersion(0x43A490), pDictionary, hash);
    }
}
