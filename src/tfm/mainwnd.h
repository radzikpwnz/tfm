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

#include "fsnode.h"
#include "toolbar.h"
#include "navbar.h"
#include "treeview.h"
#include "content_view.h"
#include "sizebar.h"
#include "statusbar.h"

// Main window
class MainWnd
{
public:
    ~MainWnd()
    {
        delete mToolbar;
        delete mNavBar;
        delete mTreeView;
        delete mContent;
        delete mSizeBar;
        delete mStatusBar;
    }

public:
    static bool initCommon();
    static MainWnd* create(HINSTANCE hInstance);

private:
    static bool registerWindowClass();
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
    // Get hwnd
    HWND hwnd() { return mHWnd; }

    // Get content view
    ContentView* getContentView() { return mContent; }

    void onCurPathChange();
    void onContentUpdate();
    void onTreeExpanding(HTREEITEM treeItem, FSNode const& node);
    void onTreeRefresh();
    //void onTreeCollapsed(HTREEITEM treeItem, FSNode const& node);

    void setViewStyle(unsigned itemId);

private:
    void command(WPARAM wParam, LPARAM lParam);
    void resize();
    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND mHWnd;            // hwnd
    Toolbar* mToolbar;     // toolbar
    NavBar* mNavBar;       // navigation bar
    TreeView* mTreeView;   // tree view
    ContentView* mContent; // content view
    SizeBar* mSizeBar;     // size bar (between tree view and content view)
    StatusBar* mStatusBar; // status bar
};