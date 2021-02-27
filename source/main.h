/*
 * Definitions for HTH3Chess
 * Created on 2021/02/15 by thearst3rd
 */

#pragma once

#include <citro2d.h>

#define SCREEN_TOP_WIDTH 		400
#define SCREEN_BOTTOM_WIDTH 	320
#define SCREEN_HEIGHT 			240

typedef struct
{
	void (*init)(void *arg);
	void (*deinit)();
	void (*update)();
	void (*drawTop)(gfx3dSide_t side);
	void (*drawBottom)();
} appState;
