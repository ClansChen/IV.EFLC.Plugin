#include "stdinc.h"
#include "plugin.h"
#include "font.h"
#include "table.h"
#include "game.h"

static const char * __stdcall GetTextFileName(int)
{
    return "CHINESE.GXT";
}

namespace Plugin
{
    void Patch()
    {
        injector::MakeCALL(Game::AddressByVersion(0x88A690 + 0x100), Font::GetStringWidthHook);

        injector::MakeJMP(Game::AddressByVersion(0x8859E0), Font::SkipAWord);

        injector::MakeCALL(Game::AddressByVersion(0x884A90 + 0xD4), Font::GetCharacterSizeNormalDispatch);
        injector::MakeCALL(Game::AddressByVersion(0x88A690 + 0x145), Font::GetCharacterSizeNormalDispatch);
        injector::MakeCALL(Game::AddressByVersion(0x8D32D0 + 0x535), Font::GetCharacterSizeNormalDispatch);

        injector::MakeCALL(Game::AddressByVersion(0x884BC0 + 0x14A), Font::GetCharacterSizeDrawingDispatch);
        injector::MakeCALL(Game::AddressByVersion(0x88A58B), Font::GetCharacterSizeDrawingDispatch);

        injector::MakeCALL(Game::AddressByVersion(0x884BC0 + 0x142), Font::PrintCharDispatch);
        injector::MakeCALL(Game::AddressByVersion(0x88A010 + 0x490), Font::PrintCharDispatch);

        injector::MakeCALL(Game::AddressByVersion(0x887642), Font::LoadTextureCB);
        injector::MakeCALL(Game::AddressByVersion(0x887CB6), Font::LoadTextureCB);

        injector::MakeJMP(Game::AddressByVersion(0x8A3880), GetTextFileName);

        //Esc菜单鼠标变色坐标
    }

    void Init(HMODULE module)
    {
        wchar_t PluginPath[512];
        GetModuleFileNameW(module, PluginPath, 512);
        Table::LoadTable(std::experimental::filesystem::v1::path{ PluginPath }.parent_path() / "wmhhz/table.dat");

        Patch();
    }
}
