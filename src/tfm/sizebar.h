#pragma once

#include "common.h"

#include "mainwnd_fwd.h"

class SizeBar
{
public:
    HWND hwnd() { return mHWnd; }

public:
    static SizeBar* create(HINSTANCE hInstance, MainWnd* parentWnd);

    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void setSizedControls(HWND hwnd1, HWND hwnd2)
    {
        mHWndCtrl1 = hwnd1;
        mHWndCtrl2 = hwnd2;
    }

private:
    void setMouseTracking();

private:
    HWND mHWnd;
    MainWnd* parentWnd;
    WNDPROC mOrigWndProc;

    HWND mHWndCtrl1;
    HWND mHWndCtrl2;
    HCURSOR mCursorArrow;
    HCURSOR mCursorLR;

    bool mIsTracking;

    bool mIsSizing;
    POINTS mCurPos;
    int mStartX;
    HWND mHPrevCapture;
};