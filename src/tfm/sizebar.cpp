#include <windows.h>

#include "common.h"

#include "mainwnd.h"

#include "sizebar.h"


static const wchar_t InstanceProp[] = L"INSTANCE";


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

LRESULT CALLBACK
SizeBar::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SizeBar* sizeBar = (SizeBar*)GetProp(hWnd, InstanceProp);
    return sizeBar->wndProcInternal(hWnd, message, wParam, lParam);
}

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

    sizeBar->parentWnd = parentWnd;
    UpdateWindow(sizeBar->mHWnd);
    return sizeBar;
}