#pragma once

#include "common.h"

#include "mainwnd_fwd.h"

// Toolbar
class Toolbar
{
private:
    // Toolbar button
    struct ToolbarBtn
    {
        wchar_t *text;            // text
        unsigned img_id;          // image id
        void(Toolbar::* proc)();  // handler
        boolean insert_sep;       // insert separator after button
    };

private:
    static ToolbarBtn buttons[];

public:
    static Toolbar* create(HINSTANCE hInstance, MainWnd* parentWnd);

private:
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
    // Get hwnd
    HWND hwnd() { return mHWnd; }

public:
    void command(unsigned command);

private:
    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    
    bool addButtons();

    void actUp();
    void actCopy();
    void actCut();
    void actPaste();
    void actDelete();
    void actNewFolder();

private:
    HWND mHWnd;            // hwnd
    MainWnd* mParentWnd;   // parent window
    WNDPROC mOrigWndProc;  // orig window procedure
};