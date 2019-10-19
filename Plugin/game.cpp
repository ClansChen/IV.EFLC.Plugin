#include "game.h"

enum class eGameVersion
{
    UNCHECKED,
    IV_1_0_8_0,
    EFLC_1_1_3_0,
    UNKNOWN
};

static eGameVersion GetGameVersion()
{
    static eGameVersion product = eGameVersion::UNCHECKED;

    if (product == eGameVersion::UNCHECKED)
    {
        switch (injector::ReadMemory<std::uint32_t>(injector::aslr_ptr(0x608C34).get(), true))
        {
        case 0x404B100F:
            product = eGameVersion::IV_1_0_8_0;
            break;

        case 0x04C1F600:
            product = eGameVersion::EFLC_1_1_3_0;
            break;

        default:
            product = eGameVersion::UNKNOWN;
            break;
        }
    }

    return product;
}

namespace Game
{
    injector::auto_pointer AddressByVersion(std::uintptr_t iv, std::uintptr_t eflc)
    {
        switch (GetGameVersion())
        {
        case eGameVersion::IV_1_0_8_0:
            return injector::aslr_ptr(iv).get();

        case eGameVersion::EFLC_1_1_3_0:
            return injector::aslr_ptr(eflc).get();

        default:
            return injector::auto_pointer();
        }
    }

    void SetRenderState(void *pTexture, int arg_4)
    {
        //80 7C 24 08 00 A1
        injector::cstd<void(void*, int)>::call(AddressByVersion(0x85CF60), pTexture, arg_4);
    }

    std::uint8_t GetRenderIndex()
    {
        //A1 ? ? ? ? 83 F8 FF 75 1E
        return injector::cstd<std::uint8_t()>::call(AddressByVersion(0x8840B0));
    }
}
