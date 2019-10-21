#include "game.h"
#include "byte_pattern.h"

namespace Game
{
    GameAddresses Addresses;

    void GetMemoryAddresses()
    {
        byte_pattern pattern_obj;

        Addresses.pGraphics_SetRenderState = pattern_obj.find_pattern("80 7C 24 08 00 A1").get_first().p();
        Addresses.pGraphics_GetRenderIndex = pattern_obj.find_pattern("A1 ? ? ? ? 83 F8 FF 75 1E").get_first().p();
        Addresses.pDictionary_GetElementByKey = pattern_obj.find_pattern("85 C9 53 55 56 57 74 40").get_first().p();
        Addresses.pHash_HashStringFromSeediCase = pattern_obj.find_pattern("8B 4C 24 08 56 8B 74 24 08 80 3E 22").get_first().p();
        Addresses.pFont_Details = *pattern_obj.find_pattern("8D 0C C5 ? ? ? ? 51 8D").get_first().p<Font::CFontDetails *>(3);
        Addresses.pFont_RenderState = *pattern_obj.find_pattern("83 C6 2C 83 C7 01 81 FE ? ? ? ?").get_first().p<Font::CFontRenderState*>(8);
        Addresses.pFont_ResolutionX = *pattern_obj.find_pattern("D8 35 (? ? ? ?) 5E D8 05").get_first().p<float*>(2);
        Addresses.pFont_PrintChar = pattern_obj.find_pattern("83 EC 34 83 3D ? ? ? ? FF").get_first().p();
        Addresses.pFont_GetCharacterSizeNormal = pattern_obj.find_pattern("83 EC 08 56 57 E8").get_first().p();
        Addresses.pFont_GetCharacterSizeDrawing = pattern_obj.find_pattern("51 A0 ? ? ? ? 0F B6 15").get_first().p();
        Addresses.pFont_Render2DPrimitive = pattern_obj.find_pattern("83 EC 10 8B 44 24 14 F3 0F 7E 00").get_first().p();
    }

    void Graphics_SetRenderState(void* texture, int arg4)
    {
        injector::cstd<void(void*, int)>::call(Addresses.pGraphics_SetRenderState, texture, arg4);
    }

    std::uint8_t Graphics_GetRenderIndex()
    {
        return injector::cstd<std::uint8_t()>::call(Addresses.pGraphics_GetRenderIndex);
    }

    void* Dictionary_GetElementByKey(void* dictionary, std::uint32_t hash)
    {
        return injector::thiscall<void* (void*, std::uint32_t)>::call(Addresses.pDictionary_GetElementByKey, dictionary, hash);
    }

    std::uint32_t Hash_HashStringFromSeediCase(const char* str, std::uint32_t hash)
    {
        return injector::cstd<std::uint32_t(const char*, std::uint32_t)>::call(Addresses.pHash_HashStringFromSeediCase, str, hash);
    }

    float Font_GetCharacterSizeNormal(std::uint16_t character)
    {
        return injector::cstd<float(std::uint16_t)>::call(Addresses.pFont_GetCharacterSizeNormal, character);
    }

    float Font_GetCharacterSizeDrawing(std::uint16_t character, bool use_extra_width)
    {
        return injector::cstd<float(std::uint16_t, bool)>::call(Addresses.pFont_GetCharacterSizeDrawing, character, use_extra_width);
    }

    void Font_Render2DPrimitive(const Font::rageRect* screen_rect, const Font::rageRect* texture_rect, std::uint32_t color, bool buffered)
    {
        injector::cstd<void(const Font::rageRect*, const Font::rageRect*, std::uint32_t, bool)>::call(Addresses.pFont_Render2DPrimitive, screen_rect, texture_rect, color, buffered);
    }

    void Font_PrintChar(float posx, float posy, std::uint16_t character, bool buffered)
    {
        injector::cstd<void(float, float, std::uint16_t, bool)>::call(Addresses.pFont_PrintChar, posx, posy, character, buffered);
    }
}
