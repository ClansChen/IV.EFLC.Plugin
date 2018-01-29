#pragma once

#define VALIDATE_SIZE(expr, size, hint) static_assert(sizeof(expr) == size, hint);

typedef unsigned int rageRGBA;

struct rageRect
{
	float field_0;
	float field_4;
	float field_8;
	float field_C;
};

class CFontDetails
{
public:
	rageRGBA field_0;
	float fScaleX;
	float fScaleY;
	float fButtonScaleX;
	int alignment;
	bool bDrawBox;
	bool field_15;
	bool bProportional;
	bool field_17;
	bool bUseUnderscore;
	bool field_19;
	bool field_1A;
	char pad1[1];
	rageRGBA BoxColor;
	float fWrapX;
	float fCentreWrapX;
	unsigned char nFont;
	unsigned char nExtraWidthIndex;
	char pad2[2];
	float fDropShadowSize;
	rageRGBA DropShadowColor;
	float fEdgeSize2;
	float fEdgeSize;
	float fLineHeight;
	float field_40;
	bool field_44;
	char pad3[3];
};
VALIDATE_SIZE(CFontDetails, 0x48, "hehe")

class CFontInfo
{
public:
	unsigned char iPropValues[255];
	unsigned char iTextureMap[255];
	char pad1[2];
	float fUnpropValue;
	float fWidthOfSpaceBetweenChars[5];
	float fJapaneseSubFont1SpaceWidth;
	float fJapaneseSubFont2SpaceWidth;
	int iMainFontStart;
	int iMainFontEnd;
	int iSubFont1Start;
	int iSubFont1End;
	int iSubFont2Start;
	int iSubFont2End;
	int iCommonFontStart;
	int iCommonFontEnd;
	void *pTexture;
	float fTextureWidth;
	float fTextureHeight;
	float fSpriteSize;
	float field_250;
	float field_254;
};
VALIDATE_SIZE(CFontInfo, 0x258, "hehe")

class CFontRenderState
{
public:
	static const unsigned int MagicNumber = 0x31337FAD;

	unsigned int magic;
	float field_4;
	float field_8;
	float fScaleX;
	float fScaleY;
	float field_14;
	rageRGBA field_18;
	float field_1C;
	int field_20;
	bool field_24;
	unsigned char nExtraWidthIndex;
	bool bProportional;
	bool field_27;
	bool bUseUnderscore;
	unsigned char nFont;
	char pad1[2];
	float fEdgeSize;
	unsigned int TokenType;
	bool field_34;
	char pad2[3];
};
VALIDATE_SIZE(CFontRenderState, 0x38, "hehe")

namespace Font
{
	bool IsNaiveCharacter(wchar_t);

	float GetCharacterSizeNormalDispatch(wchar_t);
	float GetCHSCharacterSizeNormal();

	float GetCharacterSizeDrawingDispatch(wchar_t, bool);
	float GetCHSCharacterSizeDrawing(bool);

	wchar_t *SkipAWord(wchar_t *);

	void PrintCharDispatch(float, float, wchar_t, int);
	void PrintCHSChar(float, float, wchar_t);

	void *__fastcall LoadTextureCB(void *, int, unsigned int);
	void GetStringWidthHook();
}
