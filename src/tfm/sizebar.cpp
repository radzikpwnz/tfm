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

#include "stuff.h"
#include "mainwnd.h"

#include "sizebar.h"


static const wchar_t InstanceProp[] = L"INSTANCE";


// Set mouse tracking (mouse leave event)
void
SizeBar::setMouseTracking()
{
    TRACKMOUSEEVENT tme = {};
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = mHWnd;
    TrackMouseEvent(&tme);

    mIsTracking = true;
}

// Window procedure (internal function)
LRESULT CALLBACK
SizeBar::wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch ( message )
    {
        case WM_NCHITTEST:
        {
            return HTCLIENT;
        }
        case WM_SETCURSOR:
        {
            return 1;
        }
        case WM_MOVE:
        {
            mCurPos = MAKEPOINTS(lParam);
            break;
        }
        case WM_MOUSELEAVE:
        {
            SetCursor(mCursorArrow);
            mIsTracking = false;
            break;
        }
        case WM_MOUSEMOVE:
        {
            POINTS p = MAKEPOINTS(lParam);
            if ( mIsSizing )
            {
                int dx = p.x - mStartX;
                RECT rect1, rect2;
                GetWindowRectInParent(mHWndCtrl1, &rect1);
                GetWindowRectInParent(mHWndCtrl2, &rect2);
                int width1 = rect1.right - rect1.left + dx;
                int width2 = rect2.right - rect2.left - dx;
                if ( width1 > 10 && width2 > 10 )
                {
                    SetWindowPos(mHWnd, NULL, mCurPos.x + dx, mCurPos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
                    SetWindowPos(mHWndCtrl1, NULL, rect1.left, rect1.top, rect1.right - rect1.left + dx, rect1.bottom - rect1.top, SWP_NOZORDER);
                    SetWindowPos(mHWndCtrl2, NULL, rect2.left + dx, rect2.top, rect2.right - rect2.left - dx, rect2.bottom - rect2.top, SWP_NOZORDER);
                }
            } else if ( !mIsTracking )
            {
                SetCursor(mCursorLR);
                setMouseTracking();
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            mStartX = MAKEPOINTS(lParam).x;
            mHPrevCapture = SetCapture(mHWnd);
            mIsSizing = true;
            break;
        }
        case WM_LBUTTONUP:
        {
            ReleaseCapture();
            if ( mHPrevCapture ) SetCapture(mHPrevCapture);
            mIsSizing = false;
            break;
        }
        default:
            return mOrigWndProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// Window procedure
LRESULT CALLBACK
SizeBar::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SizeBar* sizeBar = (SizeBar*)GetProp(hWnd, InstanceProp);
    return sizeBar->wndProcInternal(hWnd, message, wParam, lParam);
}

// Create sizebar instance
SizeBar*
SizeBar::create(HINSTANCE hInstance, MainWnd* parentWnd)
{
    SizeBar* sizeBar = new SizeBar();

    sizeBar->mHWnd = CreateWindowEx(0, WC_STATIC, NULL,
                                    WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,
                                    0, 0, 5, 0,
                                    parentWnd->hwnd(), (HMENU)NULL, hInstance, NULL);

    if ( sizeBar->mHWnd == NULL )
    {
        return nullptr;
    }

    sizeBar->mCursorArrow = LoadCursor(NULL, IDC_ARROW);
    sizeBar->mCursorLR = LoadCursor(NULL, IDC_SIZEWE);

    sizeBar->mOrigWndProc = (WNDPROC)SetWindowLongPtr(sizeBar->mHWnd, GWLP_WNDPROC, (LONG_PTR)wndProc);
    SetProp(sizeBar->mHWnd, InstanceProp, sizeBar);

    sizeBar->mParentWnd = parentWnd;
    UpdateWindow(sizeBar->mHWnd);
    return sizeBar;
}