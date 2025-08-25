#ifndef REVOLUTION_H
#define REVOLUTION_H

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <dinput.h>
#include "panorama.h"

// Initialize DirectInput method
LPDIRECTINPUT8 pDI;          // DirectInput interface
LPDIRECTINPUTDEVICE8 pMouse; // Mouse device

// move cursor "dx" and "dy" pixels relative to its current position
void moveMouseRelative(int dx, int dy);

// check if both Alt and Insert keys are pressed
int checkHotkeyPressed();

#endif // REVOLUTION_H