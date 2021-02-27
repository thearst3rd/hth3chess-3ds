/*
 * Definitions for the in-game state of HTH3Chess
 * Created on 2021/02/15 by thearst3rd
 */

#pragma once

#include <citro2d.h>

void stateIngameInit(void *arg);
void stateIngameDeinit();
void stateIngameUpdate();
void stateIngameDrawTop(gfx3dSide_t side);
void stateIngameDrawBottom();
