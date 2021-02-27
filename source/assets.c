/*
 * Implementation for loading in game assets
 * Created on 2021/02/15 by thearst3rd
 */

#include "assets.h"
#include "sprites.h"

C2D_SpriteSheet spriteSheet;
gfxPieceSet pieceSet;
C2D_Sprite checkIndicator;

// Define board colors
u32 cBackground;
u32 cDarkSq;
u32 cLightSq;
u32 cHighlightSq;
u32 cPieceTransparent;
u32 cLegalMove;

C2D_ImageTint tintPieceTransparent;

void initColors()
{
	cBackground = C2D_Color32(25, 25, 25, 255);
	cDarkSq = C2D_Color32(167, 129, 177, 255);
	cLightSq = C2D_Color32(234, 223, 237, 255);
	cHighlightSq = C2D_Color32(255, 255, 0, 100);
	cPieceTransparent = C2D_Color32(255, 255, 255, 70);
	cLegalMove = C2D_Color32(0, 0, 0, 100);

	tintPieceTransparent = (C2D_ImageTint)
	{
		(C2D_Tint) { cPieceTransparent, 0.0 },
		(C2D_Tint) { cPieceTransparent, 0.0 },
		(C2D_Tint) { cPieceTransparent, 0.0 },
		(C2D_Tint) { cPieceTransparent, 0.0 }
	};
}

void initSprite(C2D_Sprite *spr, int index)
{
	C2D_SpriteFromSheet(spr, spriteSheet, index);
	C2D_SpriteSetCenter(spr, 0.5f, 0.5f);
	C2D_SpriteSetPos(spr, 0, 0);
	C2D_SpriteSetRotation(spr, C3D_Angle(0));
}

void initPieceSet()
{
	initSprite(&pieceSet.wK, sprites_wK_idx);
	initSprite(&pieceSet.wQ, sprites_wQ_idx);
	initSprite(&pieceSet.wR, sprites_wR_idx);
	initSprite(&pieceSet.wB, sprites_wB_idx);
	initSprite(&pieceSet.wN, sprites_wN_idx);
	initSprite(&pieceSet.wP, sprites_wP_idx);
	initSprite(&pieceSet.bK, sprites_bK_idx);
	initSprite(&pieceSet.bQ, sprites_bQ_idx);
	initSprite(&pieceSet.bR, sprites_bR_idx);
	initSprite(&pieceSet.bB, sprites_bB_idx);
	initSprite(&pieceSet.bN, sprites_bN_idx);
	initSprite(&pieceSet.bP, sprites_bP_idx);
}

void loadSprites()
{
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet)
		svcBreak(USERBREAK_PANIC);

	// Initialize sprites
	initPieceSet();
	C2D_SpriteFromSheet(&checkIndicator, spriteSheet, sprites_check_idx);
	C2D_SpriteSetCenter(&checkIndicator, 0.0f, 0.0f);
	C2D_SpriteSetPos(&checkIndicator, 0, 0);
	C2D_SpriteSetRotation(&checkIndicator, C3D_Angle(0));
}

void freeSprites()
{
	C2D_SpriteSheetFree(spriteSheet);
}
