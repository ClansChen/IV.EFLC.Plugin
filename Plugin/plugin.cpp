#include "plugin.h"
#include "font.h"
#include "table.h"

static const char * __stdcall GetTextFileName(int)
{
	return "CHINESE.GXT";
}

namespace Plugin
{
	eGameVersion GetGameVersion()
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

	injector::auto_pointer AddressByVersion(std::uintptr_t addressiv, std::uintptr_t addresseflc)
	{
		switch (GetGameVersion())
		{
		case eGameVersion::IV_1_0_8_0:
			return injector::aslr_ptr(addressiv).get();

		case eGameVersion::EFLC_1_1_3_0:
			return injector::aslr_ptr(addresseflc).get();

		default:
			return injector::auto_pointer();
		}

	}

	void Patch()
	{
		injector::MakeCALL(AddressByVersion(0x88A690 + 0x100), Font::GetStringWidthHook);

		injector::MakeJMP(AddressByVersion(0x8859E0), Font::SkipAWord);

		injector::MakeCALL(AddressByVersion(0x884A90 + 0xD4), Font::GetCharacterSizeNormalDispatch);
		injector::MakeCALL(AddressByVersion(0x88A690 + 0x145), Font::GetCharacterSizeNormalDispatch);
		injector::MakeCALL(AddressByVersion(0x8D32D0 + 0x535), Font::GetCharacterSizeNormalDispatch);

		injector::MakeCALL(AddressByVersion(0x884BC0 + 0x14A), Font::GetCharacterSizeDrawingDispatch);
		injector::MakeCALL(AddressByVersion(0x88A58B), Font::GetCharacterSizeDrawingDispatch);

		injector::MakeCALL(AddressByVersion(0x884BC0 + 0x142), Font::PrintCharDispatch);
		injector::MakeCALL(AddressByVersion(0x88A010 + 0x490), Font::PrintCharDispatch);

		injector::MakeCALL(AddressByVersion(0x887642), Font::LoadTextureCB);
		injector::MakeCALL(AddressByVersion(0x887CB6), Font::LoadTextureCB);

		injector::MakeJMP(Plugin::AddressByVersion(0x8A3880), GetTextFileName);

		//主菜单排版
	}

	void Init(HMODULE module)
	{
		wchar_t PluginPath[512];
		GetModuleFileNameW(module, PluginPath, 512);
		Table::LoadTable(std::experimental::filesystem::v1::path{ PluginPath }.parent_path() / "wmhhz/table.dat");

		Patch();
	}
}
