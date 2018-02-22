#include "game.h"
#include "plugin.h"

namespace Game
{
	void SetRenderState(void *pTexture, int arg_4)
	{
		injector::cstd<void(void*, int)>::call(Plugin::AddressByVersion(0x85CF60), pTexture, arg_4);
	}

	std::uint8_t GetRenderIndex()
	{
		return injector::cstd<std::uint8_t()>::call(Plugin::AddressByVersion(0x8840B0));
	}
}
