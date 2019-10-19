#include "hash.h"
#include "game.h"

namespace Hash
{
    std::uint32_t HashStringFromSeediCase(const char *str, std::uint32_t seed)
    {
        //8B 4C 24 08 56 8B 74 24 08 80 3E 22
        return injector::cstd<std::uint32_t(const char *, std::uint32_t)>::call(Game::AddressByVersion(0x45CF50), str, seed);
    }
}
