#include "plugin.h"
#include "font.h"
#include "table.h"
#include "game.h"
#include "byte_pattern.h"

static const char * __stdcall GetTextFileName(int)
{
    return "CHINESE.GXT";
}

namespace Plugin
{
    void Patch()
    {
        byte_pattern pattern_obj;

        Game::GetMemoryAddresses();

        //0F B7 03 66 3D 20 00
        injector::MakeCALL(pattern_obj.find_pattern("0F B7 03 66 3D 20 00").get_first().i(), Font::GetStringWidthHook);

        //8B 44 24 04 85 C0 75 01 C3 0F B7 08
        injector::MakeJMP(pattern_obj.find_pattern("8B 44 24 04 85 C0 75 01 C3 0F B7 08").get_first().i(), Font::SkipAWord);

        //83 C0 E0 50 (E8 ? ? ? ?) D8 407
        injector::MakeCALL(pattern_obj.find_pattern("83 C0 E0 50 E8 ? ? ? ? D8 407").get_first().i(4), Font::GetCharacterSizeNormalDispatch);
        //83 C0 E0 50 (E8 ? ? ? ?) D8 44 24 18
        injector::MakeCALL(pattern_obj.find_pattern("83 C0 E0 50 E8 ? ? ? ? D8 44 24 18").get_first().i(4), Font::GetCharacterSizeNormalDispatch);
        //66 0F BE 07 50 (E8 ? ? ? ?)
        injector::MakeCALL(pattern_obj.find_pattern("66 0F BE 07 50 E8 ? ? ? ?").get_first().i(5), Font::GetCharacterSizeNormalDispatch);

        //6A 01 57 (E8 ? ? ? ?) D8 06
        injector::MakeCALL(pattern_obj.find_pattern("6A 01 57 E8 ? ? ? ? D8 06").get_first().i(3), Font::GetCharacterSizeDrawingDispatch);
        //(E8 ? ? ? ?) D8 44 24 18 83 C4 08 D9 5C 24 10 66 85 F6
        injector::MakeCALL(pattern_obj.find_pattern("E8 ? ? ? ? D8 44 24 18 83 C4 08 D9 5C 24 10 66 85 F6").get_first().i(), Font::GetCharacterSizeDrawingDispatch);

        //(E8 ? ? ? ?) 6A 01 57 E8 ? ? ? ? D8 06
        injector::MakeCALL(pattern_obj.find_pattern("E8 ? ? ? ? 6A 01 57 E8 ? ? ? ? D8 06").get_first().i(), Font::PrintCharDispatch);
        //(E8 ? ? ? ?) 8B 1D ? ? ? ? 83 C4 10 85 DB
        injector::MakeCALL(pattern_obj.find_pattern("E8 ? ? ? ? 8B 1D ? ? ? ? 83 C4 10 85 DB").get_first().i(), Font::PrintCharDispatch);

        //8B CE (E8 ? ? ? ?) 80 3D ? ? ? ? 6A
        //两处
        injector::MakeCALL(pattern_obj.find_pattern("8B CE E8 ? ? ? ? 80 3D ? ? ? ? 6A").get(0).i(2), Font::LoadTextureCB);
        injector::MakeCALL(pattern_obj.get(1).i(2), Font::LoadTextureCB);

        //80 7C 24 04 00 56 8B F1 74 07
        injector::MakeJMP(pattern_obj.find_pattern("80 7C 24 04 00 56 8B F1 74 07").get_first().i(), GetTextFileName);

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
