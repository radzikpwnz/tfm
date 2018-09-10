#pragma once

#include "common.h"

#include "mainwnd_fwd.h"

class Toolbar
{
private:
    struct ToolbarBtn
    {
        wchar_t *text;
        unsigned img_id;
        void(Toolbar::* proc)();
        boolean insert_sep;
    };

private:
    static ToolbarBtn buttons[];

public:
    HWND hwnd() { return mHWnd; }

public:
    static Toolbar* create(HINSTANCE hInstance, MainWnd* parentWnd);
    void command(unsigned command);

private:
    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    bool addButtons();

    void actDummy();
    void actUp();
    void actCopy();
    void actCut();
    void actPaste();
    void actDelete();
    void actNewFolder();

private:
    HWND mHWnd;
    WNDPROC mOrigWndProc;
    MainWnd* mParentWnd;
};