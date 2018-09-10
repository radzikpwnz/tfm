#pragma once

#include "common.h"

#include "toolbar.h"
#include "navbar.h"
#include "treeview.h"
#include "content_view.h"
#include "sizebar.h"
#include "statusbar.h"

class MainWnd
{
public:
    HWND hwnd() { return mHWnd; }

    ~MainWnd()
    {
        delete mToolbar;
        delete mTreeView;
        delete mContent;
        delete mStatusBar;
    }

public:
    ContentView* getContentView() { return mContent; }

    void onCurPathChange();
    void onContentUpdate();
    void onTreeExpanding(HTREEITEM treeItem, FSNode const& node);
    void onTreeRefresh();
    //void onTreeCollapsed(HTREEITEM treeItem, FSNode const& node);

    static bool initCommon();

    static MainWnd* create(HINSTANCE hInstance);

    void setViewStyle(unsigned itemId);

private:
    void command(WPARAM wParam, LPARAM lParam);
    void resize(int width, int heigth);
    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    static bool registerWindowClass();
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    HWND mHWnd;
    Toolbar* mToolbar;
    NavBar* mNavBar;
    TreeView* mTreeView;
    ContentView* mContent;
    SizeBar* mSizeBar;
    StatusBar* mStatusBar;
};