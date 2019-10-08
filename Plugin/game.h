#pragma once
#include "../common/stdinc.h"

namespace Game
{
    injector::auto_pointer AddressByVersion(std::uintptr_t iv, std::uintptr_t eflc = 0);

    void SetRenderState(void *, int = 1);
    std::uint8_t GetRenderIndex();
}
