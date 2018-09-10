#pragma once

#include "common.h"

#include "fsnode.h"
#include "mainwnd_fwd.h"

class TreeView
{
public:
    // Get hwnd
    HWND hwnd() { return mHWnd; }

    void refreshContentRec(HTREEITEM parentItem, FSNode const& parentNode);
    void refreshContent();

    static TreeView* create(HINSTANCE hInstance, MainWnd* parentWnd);
    LRESULT notify(NMHDR* nmhdr);

private:
    HWND mHWnd;          // hwnd
    MainWnd* parentWnd;  // parent window
};