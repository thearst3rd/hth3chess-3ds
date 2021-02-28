/*
 * HTH3Chess - Chess implementation in C for the 3DS
 * Created on 2021/02/14 by thearst3rd
 */

#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "assets.h"
#include "state_ingame.h"

// System variables
CFG_SystemModel model = CFG_MODEL_2DS;
bool isNew3DS = false;
bool has3DScreen = false;

// Game states
extern appState stateMainMenu;
extern appState stateIngame;

// Handle application's main state machine
appState *currentState = &stateMainMenu;
appState *nextState = NULL;
void *nextStateInitArg = NULL;

bool shouldBreakFromMainLoop = false;

C3D_RenderTarget *topLeft = NULL;
C3D_RenderTarget *topRight = NULL;
C3D_RenderTarget *bottom = NULL;

extern u32 cBackground;

// Input handling
u32 kDown;
u32 kHeld;
u32 kUp;
touchPosition touch;
float slider3D = 0;


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

	// Get system configurtion
	Result rc = cfguInit();
	if (R_SUCCEEDED(rc))
	{
		CFG_SystemModel tModel;
		bool tIsNew3DS;
		u8 tIs2DS;

		rc = CFGU_GetSystemModel(&tModel);
		if (R_SUCCEEDED(rc))
			model = tModel;

		rc = APT_CheckNew3DS(&tIsNew3DS);
		if (R_SUCCEEDED(rc))
			isNew3DS = tIsNew3DS;

		rc = CFGU_GetModelNintendo2DS(&tIs2DS);
		if (R_SUCCEEDED(rc))
			has3DScreen = (tIs2DS == 1); 	// 1 means not 2DS...

		//isNew3DS = (model == CFG_MODEL_N2DSXL) || (model == CFG_MODEL_N3DS) || (model == CFG_MODEL_N3DSXL);
		//has3DScreen = (model == CFG_MODEL_3DS) || (model == CFG_MODEL_3DSXL) || (model == CFG_MODEL_N3DS) || (model == CFG_MODEL_N3DSXL);

		cfguExit();
	}

	if (isNew3DS)
		osSetSpeedupEnable(true);

	srand(time(NULL));

	// Create screens
	topLeft = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	if (has3DScreen)
	{
		topRight = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
		gfxSet3D(true);
	}
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	// Load assets
	initColors();
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

		if (has3DScreen)
			slider3D = osGet3DSliderState();

		currentState->update();

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(topLeft, cBackground);
		C2D_SceneBegin(topLeft);
		currentState->drawTop(GFX_LEFT);
		if (has3DScreen)
		{
			C2D_TargetClear(topRight, cBackground);
			C2D_SceneBegin(topRight);
			currentState->drawTop(GFX_RIGHT);
		}
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
