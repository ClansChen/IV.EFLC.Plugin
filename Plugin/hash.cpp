#include "hash.h"
#include "game.h"

namespace Hash
{
    std::uint32_t HashStringFromSeediCase(const char *str, std::uint32_t seed)
    {
        return injector::cstd<std::uint32_t(const char *, std::uint32_t)>::call(Game::AddressByVersion(0x45CF50), str, seed);
    }
}
