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
        //0F B7 03 66 3D 20 00
        injector::MakeCALL(Game::AddressByVersion(0x88A690 + 0x100), Font::GetStringWidthHook);

        //8B 44 24 04 85 C0 75 01 C3 0F B7 08
        injector::MakeJMP(Game::AddressByVersion(0x8859E0), Font::SkipAWord);

        //83 C0 E0 50 (E8 ? ? ? ?) D8 407
        injector::MakeCALL(Game::AddressByVersion(0x884A90 + 0xD4), Font::GetCharacterSizeNormalDispatch);
        //83 C0 E0 50 (E8 ? ? ? ?) D8 44 24 18
        injector::MakeCALL(Game::AddressByVersion(0x88A690 + 0x145), Font::GetCharacterSizeNormalDispatch);
        //66 0F BE 07 50 (E8 ? ? ? ?)
        injector::MakeCALL(Game::AddressByVersion(0x8D32D0 + 0x535), Font::GetCharacterSizeNormalDispatch);

        //6A 01 57 (E8 ? ? ? ?) D8 06
        injector::MakeCALL(Game::AddressByVersion(0x884BC0 + 0x14A), Font::GetCharacterSizeDrawingDispatch);
        //(E8 ? ? ? ?) D8 44 24 18 83 C4 08 D9 5C 24 10 66 85 F6
        injector::MakeCALL(Game::AddressByVersion(0x88A58B), Font::GetCharacterSizeDrawingDispatch);

        //(E8 ? ? ? ?) 6A 01 57 E8 ? ? ? ? D8 06
        injector::MakeCALL(Game::AddressByVersion(0x884BC0 + 0x142), Font::PrintCharDispatch);
        //(E8 ? ? ? ?) 8B 1D ? ? ? ? 83 C4 10 85 DB
        injector::MakeCALL(Game::AddressByVersion(0x88A010 + 0x490), Font::PrintCharDispatch);

        //8B CE (E8 ? ? ? ?) 80 3D ? ? ? ? 6A
        //两处
        injector::MakeCALL(Game::AddressByVersion(0x887642), Font::LoadTextureCB);
        injector::MakeCALL(Game::AddressByVersion(0x887CB6), Font::LoadTextureCB);

        //80 7C 24 04 00 56 8B F1 74 07
        injector::MakeJMP(Game::AddressByVersion(0x8A3880), GetTextFileName);

        //Esc菜单鼠标变色坐标

        //存档名字只取一个字节

        //不同语言的字符设置
    }

    void Init(HMODULE module)
    {
        wchar_t PluginPath[512];
        GetModuleFileNameW(module, PluginPath, 512);
        globalTable.LoadTable(std::filesystem::path{ PluginPath }.parent_path() / "GTAIV.CHS/table.dat");

        Patch();
    }
}
