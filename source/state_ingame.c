/*
 * Implementation for the in-game state of HTH3Chess
 * Created on 2021/02/15 by thearst3rd
 */

#include "state_ingame.h"

#include <stdlib.h>

#include <chesslib/chess.h>

#include "main.h"
#include "assets.h"


#define SQUARE_SIZE 	30
#define BOARD_OFFSET_X 	40
#define BOARD_OFFSET_Y 	0

extern int shouldBreakFromMainLoop;
extern C3D_RenderTarget *bottom;

// Get graphics variables
extern gfxPieceSet pieceSet;
extern C2D_Sprite checkIndicator;

extern u32 cBackground;
extern u32 cDarkSq;
extern u32 cLightSq;
extern u32 cHighlightSq;
extern u32 cPieceTransparent;
extern u32 cLegalMove;

extern C2D_ImageTint tintPieceTransparent;

// Input handling
extern u32 kDown;
extern u32 kHeld;
extern u32 kUp;
extern touchPosition touch;

// Define game variables
chess *c = NULL;

int isFlipped = 0;
int isDragging = 0;
int autoFlip = 0;
int botEnabled = 0;
int showLegals = 1;

sq draggingSq = SQ_INVALID;
sq hoverSq = SQ_INVALID;
sq highlightSq1 = SQ_INVALID;
sq highlightSq2 = SQ_INVALID;

sqSet legalMoves;
sqSet checkMask;

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

// Returns the sprite graphic for the given piece
C2D_Sprite *pieceGetSprite(piece p)
{
	switch (p)
	{
		case pWKing: 	return &pieceSet.wK;
		case pWQueen: 	return &pieceSet.wQ;
		case pWRook: 	return &pieceSet.wR;
		case pWBishop: 	return &pieceSet.wB;
		case pWKnight: 	return &pieceSet.wN;
		case pWPawn: 	return &pieceSet.wP;
		case pBKing: 	return &pieceSet.bK;
		case pBQueen: 	return &pieceSet.bQ;
		case pBRook: 	return &pieceSet.bR;
		case pBBishop: 	return &pieceSet.bB;
		case pBKnight: 	return &pieceSet.bN;
		case pBPawn: 	return &pieceSet.bP;
		default: 		return NULL;
	}
}

void drawPiece(piece p, int x, int y, int tinted)
{
	C2D_Sprite *spr = pieceGetSprite(p);
	if (!spr)
		return;

	C2D_SpriteSetPos(spr, x, y);
	if (tinted)
		C2D_DrawSpriteTinted(spr, &tintPieceTransparent);
	else
		C2D_DrawSprite(spr);
}

void updateGameStatus()
{
	char *status;
	char *moves;

	switch (chessGetTerminalState(c))
	{
		case tsOngoing:
			status = chessGetPlayer(c) == pcWhite ? "White to move" : "Black to move";
			break;
		case tsCheckmate:
			status = chessGetPlayer(c) == pcWhite ? "Black wins by checkmate" : "White wins by checkmate";
			break;
		case tsDrawStalemate:
			status = "Draw by stalemate";
			break;
		case tsDrawClaimed50MoveRule:
			status = "Draw claimed by 50 move rule";
			break;
		case tsDraw75MoveRule:
			status = "Draw by 75 move rule";
			break;
		case tsDrawClaimedThreefold:
			status = "Draw claimed by threefold repetition";
			break;
		case tsDrawFivefold:
			status = "Draw by fivefold repetition";
			break;
		case tsDrawInsufficient:
			status = "Draw by insufficient material";
			break;
		default:
			status = "Error: unknown status";
	}

	moves = chessGetMoveHistoryUci(c);

	consoleClear();

	printf("\x1b[2;1H");
	if (botEnabled)
		printf("Bot enabled   ");

	if (autoFlip && chessGetTerminalState(c) == tsOngoing)
	{
		printf("Auto-Flip enabled   ");
		isFlipped = chessGetPlayer(c) == pcBlack;
	}

	printf("\x1b[1;1H%s", status);
	printf("\x1b[4;1H%s", moves);

	free(moves);

	// Update highlighted squares
	if (chessGetMoveHistory(c)->tail)
	{
		move m = chessGetMoveHistory(c)->tail->move;
		highlightSq1 = m.to;
		highlightSq2 = m.from;
	}
	else
	{
		highlightSq1 = SQ_INVALID;
		highlightSq2 = SQ_INVALID;
	}

	// Update check mask
	checkMask = 0;
	for (int ind = 0; ind < 64; ind++)
	{
		sq s = sqIndex(ind);
		if (pieceGetType(chessGetPiece(c, s)) == ptKing && chessIsSquareAttacked(c, s))
			sqSetSet(&checkMask, s, 1);
	}
}

void initChess()
{
	if (c)
		chessFree(c);

	c = chessCreate();
	updateGameStatus();
}

// Given an x, y position on screen, return which board square that is in
sq posToSquare(int x, int y)
{
	if (x < BOARD_OFFSET_X
			|| x > (BOARD_OFFSET_X + 8 * SQUARE_SIZE)
			|| y < BOARD_OFFSET_Y
			|| y > (BOARD_OFFSET_Y + 8 * SQUARE_SIZE))
		return SQ_INVALID;

	sq s = sqI(1 + (x - BOARD_OFFSET_X) / SQUARE_SIZE, 8 - (y - BOARD_OFFSET_Y) / SQUARE_SIZE);
	if (isFlipped)
	{
		s.file = 9 - s.file;
		s.rank = 9 - s.rank;
	}
	return s;
}

move botGetMove()
{
	moveList *moves = chessGetLegalMoves(c);
	int index = rand() % moves->size;
	return moveListGet(moves, index);
}

void botPlayMove()
{
	if (chessGetTerminalState(c) != tsOngoing)
		return;

	chessPlayMove(c, botGetMove());
	updateGameStatus();
}


////////////////////
// MAIN CALLBACKS //
////////////////////

void stateIngameInit()
{
	initChess();
}

void stateIngameDeinit()
{
	chessFree(c);
	c = NULL;
}

void stateIngameUpdate()
{
	if (kDown & KEY_START)
		shouldBreakFromMainLoop = 1; // break in order to return to hbmenu

	if (kDown & KEY_SELECT)
	{
		if (!isDragging)
			initChess();
	}

	if (kDown & KEY_A)
	{
		if (!isDragging)
			botPlayMove();
	}

	if (kDown & KEY_B)
	{
		if (!isDragging)
		{
			chessUndo(c);
			updateGameStatus();
		}
	}

	if (kDown & KEY_X)
	{
		autoFlip = !autoFlip;
		updateGameStatus();
	}

	if (kDown & KEY_Y)
	{
		botEnabled = !botEnabled;
		updateGameStatus();
	}

	if (kDown & KEY_R)
		isFlipped = !isFlipped;

	if (kDown & KEY_L)
		showLegals = !showLegals;

	if (kDown & KEY_TOUCH)
	{
		draggingSq = posToSquare(touch.px, touch.py);
		piece p = chessGetPiece(c, draggingSq);
		if (!sqEq(draggingSq, SQ_INVALID)
				&& chessGetTerminalState(c) == tsOngoing
				&& (p != pEmpty)
				&& pieceGetColor(p) == chessGetPlayer(c))
		{
			isDragging = 1;
			legalMoves = 0;
			for (moveListNode *n = chessGetLegalMoves(c)->head; n; n = n->next)
			{
				move m = n->move;
				if (sqEq(m.from, draggingSq))
					sqSetSet(&legalMoves, m.to, 1);
			}
		}
		else
			draggingSq = SQ_INVALID;
	}

	if (kHeld & KEY_TOUCH)
	{
		if (isDragging)
			hoverSq = posToSquare(touch.px, touch.py);
	}

	if (kUp & KEY_TOUCH)
	{
		if (isDragging)
		{
			isDragging = 0;
			if (!sqEq(hoverSq, SQ_INVALID) && !sqEq(hoverSq, draggingSq))
			{
				move m = moveSq(draggingSq, hoverSq);
				if (pieceGetType(chessGetPiece(c, draggingSq)) == ptPawn && (hoverSq.rank == 1 || hoverSq.rank == 8))
					m.promotion = ptQueen;
				if (!chessPlayMove(c, m))
				{
					updateGameStatus();

					if (botEnabled)
						botPlayMove();
				}
			}
			draggingSq = SQ_INVALID;
		}
	}
}

void stateIngameDraw()
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(bottom, cBackground);
	C2D_SceneBegin(bottom);
	for (int rank = 8; rank > 0; rank--)
	{
		int rankVisual = isFlipped ? 9 - rank : rank;
		int y = (8 - rankVisual) * SQUARE_SIZE + BOARD_OFFSET_Y;
		for (int file = 1; file <= 8; file++)
		{
			int fileVisual = isFlipped ? 9 - file : file;
			int x = (fileVisual - 1) * SQUARE_SIZE + BOARD_OFFSET_X;
			sq s = sqI(file, rank);

			u32 col = sqIsDark(s) ? cDarkSq : cLightSq;
			C2D_DrawRectangle(x, y, 0, SQUARE_SIZE, SQUARE_SIZE, col, col, col, col);

			if (sqEq(s, highlightSq1) || sqEq(s, highlightSq2))
				C2D_DrawRectangle(x, y, 0, SQUARE_SIZE, SQUARE_SIZE, cHighlightSq, cHighlightSq, cHighlightSq,
						cHighlightSq);

			if (sqSetGet(&checkMask, s))
			{
				C2D_SpriteSetPos(&checkIndicator, x, y);
				C2D_DrawSprite(&checkIndicator);
			}

			piece p = chessGetPiece(c, s);
			if (p)
				drawPiece(p, x + (SQUARE_SIZE / 2), y + (SQUARE_SIZE / 2), sqEq(draggingSq, s));
		}
	}
	if (isDragging)
	{
		// According to the 2d shapes example, we should draw all circles AFTER all non-circles. That's the reason
		// for the second for loop. We do want to draw the dragging piece after, though
		if (showLegals)
		{
			for (int rank = 8; rank > 0; rank--)
			{
				int rankVisual = isFlipped ? 9 - rank : rank;
				int y = (8 - rankVisual) * SQUARE_SIZE + BOARD_OFFSET_Y;
				for (int file = 1; file <= 8; file++)
				{
					int fileVisual = isFlipped ? 9 - file : file;
					int x = (fileVisual - 1) * SQUARE_SIZE + BOARD_OFFSET_X;

					sq s = sqI(file, rank);

					if (sqSetGet(&legalMoves, s))
					{
						piece p = chessGetPiece(c, s);

						int radius;
						if (p == pEmpty)
							radius = 5;
						else
							radius = 12;

						C2D_DrawCircle(x + (SQUARE_SIZE / 2), y + (SQUARE_SIZE / 2), 0, radius, cLegalMove,
								cLegalMove, cLegalMove, cLegalMove);
					}
				}
			}
		}

		piece p = chessGetPiece(c, draggingSq);
		drawPiece(p, touch.px, touch.py, 0);
	}
	C3D_FrameEnd(0);
}
