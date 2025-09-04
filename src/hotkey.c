#include "hotkey.h"

int checkRevolutionHotkeyPressed()
{
    // Check if Alt key (VK_MENU) and Insert (VK_INSERT) are pressed
    return (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
}

int checkMultiRevolutionHotkeyPressed()
{
    // Check if Alt key (VK_MENU), Shift (VK_SHIFT) and Insert (VK_INSERT) are pressed
    return (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
}

int checkLowValueHotkeyPressed()
{

    // Alt + Shift + Plus
    return (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) != 0;
}

int checkHighValueHotkeyPressed()
{

    // Alt + Shift + Minus
    return(GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) != 0;
}

int checkUndoHotkeyPressed()
{

    // Alt + Shift + Backspace
    return (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_BACK) & 0x8000) != 0;
}

int checkExitHotkeyPressed()
{

    // Alt + Shift + '\' (Backslash)
    return (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_OEM_5) & 0x8000) != 0;
}

