#include "common.h"


// Get window rect in parent window
void
GetWindowRectInParent(HWND hwnd, RECT* rect)
{
    GetWindowRect(hwnd, rect);
    MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)rect, 2);
}