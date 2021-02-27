/*
 * HTH3Chess - Chess implementation in C for the 3DS
 * Created on 2021/02/14 by thearst3rd
 */

#include "main.h"

//#include <assert.h>
//#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "assets.h"
#include "state_ingame.h"

// Game states
extern appState stateIngame;

// Handle application's main state machine
appState *currentState = &stateIngame;
appState *nextState = NULL;
void *nextStateInitArg = NULL;

// Define board colors
u32 cBackground;
u32 cDarkSq;
u32 cLightSq;
u32 cHighlightSq;
u32 cPieceTransparent;
u32 cLegalMove;

C2D_ImageTint tintPieceTransparent;

int shouldBreakFromMainLoop = 0;

C3D_RenderTarget *top = NULL;
C3D_RenderTarget *bottom = NULL;

// Input handling
u32 kDown;
u32 kHeld;
u32 kUp;
touchPosition touch;


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
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	// Load assets
	loadSprites();

	// Init app state
	currentState->init(NULL);

	// Main loop
	while (aptMainLoop())
	{
		// Run the next frame
		hidScanInput();
		kDown = hidKeysDown();
		kHeld = hidKeysHeld();
		kUp   = hidKeysUp();
		hidTouchRead(&touch);

		currentState->update();

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, cBackground);
		C2D_SceneBegin(top);
		currentState->drawTop(GFX_LEFT); 	// Only left for now
		C2D_TargetClear(bottom, cBackground);
		C2D_SceneBegin(bottom);
		currentState->drawBottom();
		C3D_FrameEnd(0);

		// Handle exiting/state changes
		if (shouldBreakFromMainLoop)
			break;

		if (nextState != NULL)
		{
			currentState->deinit();
			currentState = nextState;
			nextState = NULL;
			currentState->init(nextStateInitArg);
		}
	}

	// Deinit game state
	currentState->deinit();

	// Free assets
	freeSprites();

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
