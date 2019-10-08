#pragma once
#include "../common/stdinc.h"

namespace Font
{
    struct rageRect
    {
        float fBottomLeftX;
        float fBottomLeftY;
        float fTopRightX;
        float fTopRightY;
    };

    class CFontDetails
    {
    public:
        std::uint32_t field_0;
        float fScaleX;
        float fScaleY;
        float fButtonScaleX;
        std::int32_t alignment;
        bool bDrawBox;
        bool field_15;
        bool bProportional;
        bool field_17;
        bool bUseUnderscore;
        bool field_19;
        bool field_1A;
        std::int8_t pad1[1];
        std::uint32_t BoxColor;
        float fWrapX;
        float fCentreWrapX;
        std::uint8_t nFont;
        std::uint8_t nExtraWidthIndex;
        std::int8_t pad2[2];
        float fDropShadowSize;
        std::uint32_t DropShadowColor;
        float fEdgeSize2;
        float fEdgeSize;
        float fLineHeight;
        float field_40;
        bool field_44;
        std::int8_t pad3[3];
    };

    VALIDATE_SIZE(CFontDetails, 0x48)

    class CFontInfo
    {
    public:
        std::uint8_t iPropValues[255];
        std::uint8_t iTextureMap[255];
        std::int8_t pad1[2];
        float fUnpropValue;
        float fWidthOfSpaceBetweenChars[5];
        float fJapaneseSubFont1SpaceWidth;
        float fJapaneseSubFont2SpaceWidth;
        std::int32_t iMainFontStart;
        std::int32_t iMainFontEnd;
        std::int32_t iSubFont1Start;
        std::int32_t iSubFont1End;
        std::int32_t iSubFont2Start;
        std::int32_t iSubFont2End;
        std::int32_t iCommonFontStart;
        std::int32_t iCommonFontEnd;
        void* pTexture;
        float fTextureWidth;
        float fTextureHeight;
        float fSpriteSize;
        float field_250;
        float field_254;
    };
    VALIDATE_SIZE(CFontInfo, 0x258)

    class CFontRenderState
    {
    public:
        std::uint32_t magic;
        float field_4;
        float field_8;
        float fScaleX;
        float fScaleY;
        float field_14;
        std::uint32_t field_18;
        float field_1C;
        std::int32_t field_20;
        bool field_24;
        std::uint8_t nExtraWidthIndex;
        bool bProportional;
        bool field_27;
        bool bUseUnderscore;
        std::uint8_t nFont;
        std::int8_t pad1[2];
        float fEdgeSize;
        std::uint32_t TokenType;
        bool field_34;
        std::int8_t pad2[3];
    };
    VALIDATE_SIZE(CFontRenderState, 0x38)

    bool IsNaiveCharacter(std::uint16_t);

    float GetCharacterSizeNormalDispatch(std::uint16_t);
    float GetCHSCharacterSizeNormal();

    float GetCharacterSizeDrawingDispatch(std::uint16_t, bool);
    float GetCHSCharacterSizeDrawing(bool);

    std::uint16_t* SkipAWord(std::uint16_t*);

    void PrintCharDispatch(float, float, std::uint16_t, int);
    void PrintCHSChar(float, float, std::uint16_t);

    void* __fastcall LoadTextureCB(void*, int, std::uint32_t);
    void GetStringWidthHook();
}
