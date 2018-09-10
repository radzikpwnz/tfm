#pragma once

#include "common.h"

#include "mainwnd_fwd.h"

class StatusBar
{
public:
    HWND hwnd() { return mHWnd; }

public:
    static StatusBar* create(HINSTANCE hInstance, MainWnd* parentWnd);

private:

private:
    HWND mHWnd;
    MainWnd* parentWnd;
};