#include "game.h"
#include "plugin.h"
#include "injector/calling.hpp"

namespace Game
{
	void SetRenderState(void *pTexture, int arg_4)
	{
		injector::cstd<void(void*, int)>::call(Plugin::AddressByVersion(0x85CF60), pTexture, arg_4);
	}

	unsigned char GetRenderIndex()
	{
		return injector::cstd<unsigned char()>::call(Plugin::AddressByVersion(0x8840B0));
	}
}
