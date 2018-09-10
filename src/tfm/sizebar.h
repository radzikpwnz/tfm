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