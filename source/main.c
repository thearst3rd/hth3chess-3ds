/*
 * HTH3Chess - Chess implementation in C for the 3DS
 * Created on 2021/02/14 by thearst3rd
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gfx.h"
#include "state_ingame.h"


// Handle application's main state machine
appState currentState = STATE_NONE;
appState nextState = STATE_NONE;

// Define board colors
u32 cBackground;
u32 cDarkSq;
u32 cLightSq;
u32 cHighlightSq;
u32 cPieceTransparent;
u32 cLegalMove;

C2D_ImageTint tintPieceTransparent;

int shouldBreakFromMainLoop = 0;

C3D_RenderTarget *bottom = NULL;

// Input handling
u32 kDown;
u32 kHeld;
u32 kUp;
touchPosition touch;


//////////////////
// STATE RUNNER //
//////////////////

void chessStateInit(appState newState)
{
	switch (newState)
	{
		case STATE_INGAME: stateIngameInit(); break;
		default: printf("Invalid State %d", newState); break;
	}
	currentState = newState;
}

void chessStateDeinit()
{
	switch (currentState)
	{
		case STATE_INGAME: stateIngameDeinit(); break;
		default: printf("Invalid State %d", currentState); break;
	}
	currentState = STATE_NONE;
}

void chessStateUpdate()
{
	switch (currentState)
	{
		case STATE_INGAME: stateIngameUpdate(); break;
		default: printf("Invalid State %d", currentState); break;
	}
}

void chessStateDraw()
{
	switch (currentState)
	{
		case STATE_INGAME: stateIngameDraw(); break;
		default: printf("Invalid State %d", currentState); break;
	}
}

//////////
// MAIN //
//////////

int main(int argc, char* argv[])
{
	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_TOP, NULL);

	srand(time(NULL));

	// Define colors
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

	// Create screens
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	loadGfx();

	// Init game state
	chessStateInit(STATE_INGAME);

	// Main loop
	while (aptMainLoop())
	{
		// Run the next frame
		hidScanInput();
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();
		kUp   = hidKeysUp();
		hidTouchRead(&touch);

		chessStateUpdate();

		// Render the scene
		chessStateDraw();

		// Handle exiting/state changes
		if (shouldBreakFromMainLoop)
			break;

		if (nextState != STATE_NONE)
		{
			chessStateDeinit();
			chessStateInit(nextState);
			nextState = STATE_NONE;
		}
	}

	// Deinit game state
	chessStateDeinit();

	// Delete graphics
	freeGfx();

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
