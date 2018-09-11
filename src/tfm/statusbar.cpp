// Copyright (c) Eltechs.
// All rights reserved.
//
// This code is licensed under the MIT License.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "common.h"

#include "mainwnd.h"

#include "statusbar.h"


// Create status bar instance
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