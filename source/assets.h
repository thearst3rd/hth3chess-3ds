/*
 * Definitions for loading in game assets
 * Created on 2021/02/15 by thearst3rd
 */

#pragma once

#include <citro2d.h>

#include "main.h"

typedef struct
{
	C2D_Sprite wK;
	C2D_Sprite wQ;
	C2D_Sprite wR;
	C2D_Sprite wB;
	C2D_Sprite wN;
	C2D_Sprite wP;
	C2D_Sprite bK;
	C2D_Sprite bQ;
	C2D_Sprite bR;
	C2D_Sprite bB;
	C2D_Sprite bN;
	C2D_Sprite bP;
} gfxPieceSet;

void initColors();
void loadSprites();
void freeSprites();
