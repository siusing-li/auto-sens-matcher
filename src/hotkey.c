#include "hotkey.h"

int checkRevolutionHotkeyPressed()
{
    // Check if Alt key (VK_MENU) and Insert (VK_INSERT) are pressed
    return (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_INSERT) & 0x8000);
}

int checkMultiRevolutionHotkeyPressed()
{
    // Check if Alt key (VK_MENU), Shift (VK_SHIFT) and Insert (VK_INSERT) are pressed
    return (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_INSERT) & 0x8000);
}

int checkLowValueHotkeyPressed(char **stream, char **prompt)
{

    // Alt + Shift + Up
    if ((GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_UP) & 0x8000))
    {
        *stream = *prompt;
        return 1;
    }
    else
    {
        return 0;
    }
}

int checkHighValueHotkeyPressed(char **stream, char **prompt)
{

    // Alt + Shift + Down
    if ((GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_DOWN) & 0x8000))
    {
        *stream = *prompt;
        return 1;
    }
    else
    {
        return 0;
    }
}

int checkUndoHotkeyPressed(char **stream, char **prompt)
{

    // Alt + Shift + Left
    if ((GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_LEFT) & 0x8000))
    {
        *stream = *prompt;
        return 1;
    }
    else
    {
        return 0;
    }
}

