#pragma once

#include "common.h"

#include "mainwnd_fwd.h"

class TreeView
{
public:
    HWND hwnd() { return mHWnd; }

public:
    void refreshContentRec(HTREEITEM parentItem, FSNode const& parentNode);
    void refreshContent();

    static TreeView* create(HINSTANCE hInstance, MainWnd* parentWnd);
    LRESULT notify(NMHDR* nmhdr);

private:

private:
    HWND mHWnd;
    MainWnd* parentWnd;
};