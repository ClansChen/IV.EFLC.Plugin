#include "dictionary.h"
#include "game.h"

namespace Dictionary
{
    void *GetElementByKey(void *pDictionary, std::uint32_t hash)
    {
        //85 C9 53 55 56 57 74 40
        //两个结果，选哪个都可以
        return injector::thiscall<void *(void *, std::uint32_t)>::call(Game::AddressByVersion(0x43A490), pDictionary, hash);
    }
}
