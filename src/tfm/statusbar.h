#pragma once

#include "common.h"

#include "mainwnd_fwd.h"

// Status bar
class StatusBar
{
public:
    static StatusBar* create(HINSTANCE hInstance, MainWnd* parentWnd);

public:
    // Get hwnd
    HWND hwnd() { return mHWnd; }

    // Set text
    void setText(std::wstring text) { SetWindowText(mHWnd, text.c_str()); }

private:
    HWND mHWnd;          // hwnd
    MainWnd* parentWnd;  // parent window
};