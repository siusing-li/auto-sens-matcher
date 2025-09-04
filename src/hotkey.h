#ifndef HOTKEY_H
#define HOTKEY_H

#include <dinput.h>
#include <stdio.h>
#include <signal.h>

// hotkey for doing a revolution
// check if both Alt and Insert keys are pressed
int checkRevolutionHotkeyPressed();

// hotkey for doing multiple revolutions
int checkMultiRevolutionHotkeyPressed();

// hotkey for increasing the current value in binary search
int checkLowValueHotkeyPressed();

// hotkey for decreasing the current value in binary search
int checkHighValueHotkeyPressed();

// hotkey for undoing the previous action in binary search
int checkUndoHotkeyPressed();

// hotkey for exiting the program
int checkExitHotkeyPressed();

#endif // HOTKEY_H