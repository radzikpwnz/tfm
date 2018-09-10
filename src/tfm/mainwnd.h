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