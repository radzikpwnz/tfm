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

#pragma once

#include "common.h"

#include "mainwnd_fwd.h"

// Size bar (left-right)
class SizeBar
{
public:
    // Get hwnd
    HWND hwnd() { return mHWnd; }

public:
    static SizeBar* create(HINSTANCE hInstance, MainWnd* parentWnd);

private:
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
    // Set sized controls (left and right)
    void setSizedControls(HWND hwnd1, HWND hwnd2)
    {
        mHWndCtrl1 = hwnd1;
        mHWndCtrl2 = hwnd2;
    }

private:
    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void setMouseTracking();

private:
    HWND mHWnd;              // hwnd
    MainWnd* mParentWnd;     // parent window
    WNDPROC mOrigWndProc;    // orig window procedure

    HWND mHWndCtrl1;         // sized control 1 hwnd
    HWND mHWndCtrl2;         // sized control 2 hwnd
    HCURSOR mCursorArrow;    // "Arrow" cursor (default)
    HCURSOR mCursorLR;       // "Left-right" cursor (sizing)

    bool mIsTracking;        // mouse tracking enabled flag
    bool mIsSizing;          // sizing flag
    POINTS mCurPos;          // current cursor position
    int mStartX;             // cursor position on sizing start
    HWND mHPrevCapture;      // previous window that capture cursor
};