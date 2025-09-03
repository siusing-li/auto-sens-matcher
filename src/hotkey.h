#ifndef HOTKEY_H
#define HOTKEY_H

#include <dinput.h>
#include <stdio.h>
#include <signal.h>

// hotkey for doing a revolution
// check if both Alt and Insert keys are pressed
int checkRevolutionHotkeyPressed();

// hotkey for doing multiple revolutions
// check if Alt + Shift + Insert is pressed
int checkMultiRevolutionHotkeyPressed();

// hotkey for increasing the current value in binary search
// check if Alt + Shift + Up
int checkLowValueHotkeyPressed(char **stream, char **prompt);

// hotkey for decreasing the current value in binary search
// check if Alt + Shift + Down
int checkHighValueHotkeyPressed(char **stream, char **prompt);

// hotkey for undoing the previous action in binary search
// check if Alt + Shift + Left
int checkUndoHotkeyPressed(char **stream, char **prompt);

#endif // HOTKEY_H