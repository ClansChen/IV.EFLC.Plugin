#include "font.h"
#include "game.h"
#include "plugin.h"
#include "table.h"
#include "dictionary.h"
#include "hash.h"

namespace Font
{
    static const float fChsWidth = 32.0f;
    static const float fSpriteWidth = 64.0f;
    static const float fSpriteHeight = 80.0f;
    static const float fTextureResolution = 4096.0f;
    static const float fTextureRowsCount = 51.2f;
    static const float fTextureColumnsCount = 64.0f;
    static const float fRatio = 4.0f;

    static CFontDetails *pDetails = Plugin::AddressByVersion(0x11F5BC8);
    static CFontRenderState *pRenderState = Plugin::AddressByVersion(0xF38114);
    static CFontInfo *pFonts = Plugin::AddressByVersion(0x11EC930);
    static float *pFontResX = Plugin::AddressByVersion(0xF3814C);

    static void *pChsFont1;
    static void *pChsFont2;
    static void *pChsFont3;

    bool IsNaiveCharacter(std::uint16_t character)
    {
        return (character < 0x100);
    }

    void *__fastcall LoadTextureCB(void *pDictionary, int, std::uint32_t hash)
    {
        void *result = Dictionary::GetElementByKey(pDictionary, hash);

        pChsFont1 = Dictionary::GetElementByKey(pDictionary, Hash::HashStringFromSeediCase("font4"));

        return result;
    }

    std::uint16_t *SkipAWord(std::uint16_t *text)
    {
        if (!text)
        {
            return text;
        }

        std::uint16_t *begin = text;
        std::uint16_t *current = text;

        while (true)
        {
            std::uint16_t character = *current;

            if (character == ' ' || character == '~' || character == 0)
            {
                break;
            }

            if (!IsNaiveCharacter(character))
            {
                if (current == begin)
                {
                    ++current;
                }

                break;
            }
            else
            {
                ++current;
            }
        }

        return current;
    }

    float GetCHSCharacterSizeNormal()
    {
        std::uint8_t index = Game::GetRenderIndex();

        return ((fChsWidth / *pFontResX + pDetails[index].fEdgeSize2) * pDetails[index].fScaleX);
    }

    float GetCharacterSizeNormalDispatch(std::uint16_t character)
    {
        if (IsNaiveCharacter(character + 0x20))
        {
            return injector::cstd<float(std::uint16_t)>::call(Plugin::AddressByVersion(0x884110), character);
        }
        else
        {
            return GetCHSCharacterSizeNormal();
        }
    }

    float GetCHSCharacterSizeDrawing(bool useextrawidth)
    {
        float extrawidth = 0.0f;

        if (useextrawidth)
        {
            extrawidth = 1.0f;
        }

        return (((fChsWidth + extrawidth) / *pFontResX + pRenderState->fEdgeSize) * pRenderState->fScaleX);
    }

    float GetCharacterSizeDrawingDispatch(std::uint16_t character, bool useextrawidth)
    {
        if (IsNaiveCharacter(character + 0x20))
        {
            return injector::cstd<float(std::uint16_t, bool)>::call(Plugin::AddressByVersion(0x874040), character, useextrawidth);
        }
        else
        {
            return GetCHSCharacterSizeDrawing(useextrawidth);
        }
    }

    void PrintCHSChar(float posx, float posy, std::uint16_t character)
    {
        rageRect screenrect, texturerect;

        if (posy < -0.06558f || posy > 1.0f)
        {
            return;
        }

        if (-(GetCHSCharacterSizeDrawing(true) / pRenderState->fScaleX) > posx || posx > 1.0f)
        {
            return;
        }

        std::uint8_t row = Table::GetCharRow(character);
        std::uint8_t column = Table::GetCharColumn(character);

        float var_24 = fSpriteWidth / fTextureResolution;
        float var_28 = (fChsWidth / *pFontResX + pRenderState->fEdgeSize) * pRenderState->fScaleX;
        float var_2C = pRenderState->fScaleY * 0.06558f;

        screenrect.field_0 = posx;
        screenrect.field_4 = posy + var_2C;
        screenrect.field_8 = posx + var_28;
        screenrect.field_C = posy;

        //texturerect.field_C = (row - 0.045f) * 40 * 1 / 512 + 4 * 1 / 512;
        //if (texturerect.field_C > 1.0f)
        //{
        //    texturerect.field_C = 1.0f;
        //}
        //texturerect.field_4 = (row - 0.045f) * 40 * 1 / 512 + 39.5f * 1 / 512 - 0.001f + 0.0048f;
        //texturerect.field_0 = column / 16.0f;
        //texturerect.field_8 = column / 16.0f + var_24;

        texturerect.field_C = (row - 0.045f / fRatio) * fSpriteHeight / fTextureResolution + 8.0f / fTextureResolution;
        if (texturerect.field_C > 1.0f)
        {
            texturerect.field_C = 1.0f;
        }
        texturerect.field_4 = (row - 0.045f / fRatio) * fSpriteHeight / fTextureResolution + 79.0f / fTextureResolution - 0.001f / fRatio + 0.0048f / fRatio;
        texturerect.field_0 = column / fTextureColumnsCount;
        texturerect.field_8 = column / fTextureColumnsCount + var_24;

        switch (pRenderState->nFont)
        {
        case 0:
            Game::SetRenderState(pChsFont1);
            break;

        case 1:
            Game::SetRenderState(pChsFont1);
            break;

        case 3:
            Game::SetRenderState(pChsFont1);
            break;

        default:
            break;
        }

        injector::cstd<void(rageRect *, rageRect *, std::uint32_t, int)>::call(Plugin::AddressByVersion(0x884300), &screenrect, &texturerect, pRenderState->field_18, 0);
    }

    void PrintCharDispatch(float posx, float posy, std::uint16_t character, int mode)
    {
        if (pRenderState->TokenType != 0 || IsNaiveCharacter(character + 0x20))
        {
            injector::cstd<void(float, float, std::uint16_t, int)>::call(Plugin::AddressByVersion(0x8843E0), posx, posy, character, mode);
        }
        else
        {
            PrintCHSChar(posx, posy, character + 0x20);
        }
    }

    __declspec(naked) void GetStringWidthHook()
    {
        static void *retaddr;

        __asm
        {
            pop retaddr;

            movzx eax, word ptr[ebx];
            mov cl, [esp + 0x10BC];
            cmp ax, ' ';
            jz space;
            push eax;
            call IsNaiveCharacter;
            add esp, 4;
            test al, al;
            movzx eax, word ptr[ebx];
            mov cl, [esp + 0x10BC];
            jnz normal;
            jmp chs;
            
        space:
            mov edx, retaddr;
            add edx, 0xB;
            jmp edx;

        normal:
            mov edx, retaddr;
            add edx, 0x13;
            jmp edx;

        chs:
            test cl, cl;
            jnz normal;
            mov dl, [esp + 0x13];
            test dl, dl;
            jz normal;
            mov edx, retaddr;
            add edx, 0x274;
            jmp edx;
        }
    }
}
