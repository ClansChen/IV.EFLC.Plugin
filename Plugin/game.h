#pragma once
#include "../common/stdinc.h"
#include "font.h"

namespace Game
{
    struct GameAddresses
    {
        //80 7C 24 08 00 A1
        void* pGraphics_SetRenderState;
        //A1 ? ? ? ? 83 F8 FF 75 1E
        void* pGraphics_GetRenderIndex;

        //85 C9 53 55 56 57 74 40 两个结果，选哪个都可以
        void* pDictionary_GetElementByKey;

        //8B 4C 24 08 56 8B 74 24 08 80 3E 22
        void* pHash_HashStringFromSeediCase;

        //8D 0C C5 (? ? ? ?) 51 8D
        Font::CFontDetails* pFont_Details;
        //83 C6 2C 83 C7 01 81 FE (? ? ? ?)
        Font::CFontRenderState* pFont_RenderState;
        //D8 35 (? ? ? ?) 5E D8 05
        float* pFont_ResolutionX;
        //83 EC 34 83 3D ? ? ? ? FF
        void* pFont_PrintChar;
        //83 EC 08 56 57 E8
        void* pFont_GetCharacterSizeNormal;
        //51 A0 ? ? ? ? 0F B6 15
        void* pFont_GetCharacterSizeDrawing;
        //83 EC 10 8B 44 24 14 F3 0F 7E 00
        void* pFont_Render2DPrimitive;
    };

    extern GameAddresses Addresses;

    void GetMemoryAddresses();

    void Graphics_SetRenderState(void* texture, int = 1);
    std::uint8_t Graphics_GetRenderIndex();

    void* Dictionary_GetElementByKey(void* dictionary, std::uint32_t hash);

    std::uint32_t Hash_HashStringFromSeediCase(const char* str, std::uint32_t hash = 0);

    float Font_GetCharacterSizeNormal(std::uint16_t character);
    float Font_GetCharacterSizeDrawing(std::uint16_t character, bool use_extra_width);
    void Font_Render2DPrimitive(const Font::rageRect* screen_rect, const Font::rageRect* texture_rect, std::uint32_t color, bool buffered);
    void Font_PrintChar(float posx, float posy, std::uint16_t character, bool buffered);
}