#include <windows.h>

#include "common.h"

#include "mainwnd.h"

#include "statusbar.h"

StatusBar*
StatusBar::create(HINSTANCE hInstance, MainWnd* parentWnd)
{
    StatusBar* statusBar = new StatusBar();

    RECT rcClient;
    GetClientRect(parentWnd->hwnd(), &rcClient);
    statusBar->mHWnd = CreateWindowEx(0, STATUSCLASSNAME, NULL,
                                      WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,
                                      0, 0, 0, 0,
                                      parentWnd->hwnd(), (HMENU)NULL, hInstance, NULL);

    if ( statusBar->mHWnd == NULL )
    {
        return nullptr;
    }

    statusBar->parentWnd = parentWnd;
    UpdateWindow(statusBar->mHWnd);
    return statusBar;
}