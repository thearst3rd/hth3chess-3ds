/*
 * Implementation for the main menu state of HTH3Chess
 * Created on 2021/02/27 by thearst3rd
 */

#include "state_mainmenu.h"

#include "main.h"


// State handling
extern bool shouldBreakFromMainLoop;
extern appState *nextState;
extern void *nextStateInitArg;
extern appState stateIngame;

// Input handling
extern u32 kDown;
extern u32 kHeld;
extern u32 kUp;
extern touchPosition touch;
extern float slider3D;

// Text objects
C2D_TextBuf textBufStatic;
C2D_Text titleText;
C2D_Text subtitleText;

void stateMainMenuInit(void *arg)
{
	textBufStatic = C2D_TextBufNew(4096);

	C2D_TextParse(&titleText, textBufStatic, "HTH3Chess");
	C2D_TextParse(&subtitleText, textBufStatic, "Press A to start a game\nPress Start to exit");

	C2D_TextOptimize(&titleText);
	C2D_TextOptimize(&subtitleText);
}

void stateMainMenuDeinit()
{
	C2D_TextBufDelete(textBufStatic);
}

void stateMainMenuUpdate()
{
	if (kDown & KEY_START)
		shouldBreakFromMainLoop = true;

	if (kDown & KEY_A)
	{
		nextState = &stateIngame;
		nextStateInitArg = NULL;
	}
}

void stateMainMenuDrawTop(gfx3dSide_t side)
{
	C2D_DrawText(&titleText, C2D_WithColor, 8.0f, 8.0f, 0.0f, 1.0f, 1.0f, C2D_Color32(255, 255, 255, 255));
}

void stateMainMenuDrawBottom()
{
	C2D_DrawText(&subtitleText, C2D_WithColor, 8.0f, 8.0f, 0.0f, 0.5f, 0.5f, C2D_Color32(255, 255, 255, 255));
}

appState stateMainMenu =
{
	stateMainMenuInit,
	stateMainMenuDeinit,
	stateMainMenuUpdate,
	stateMainMenuDrawTop,
	stateMainMenuDrawBottom
};
