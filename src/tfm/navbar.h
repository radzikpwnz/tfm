#pragma once

#include <Windows.h>

#include "fsnode.h"
#include "mainwnd_fwd.h"

class NavBar
{
private:
    struct AddrButton
    {
        AddrButton()
        {
            hwnd = 0;
            fsnode = nullptr;
            width = 0;
            isSpecial = false;
            isHidden = true;
        }

        HWND hwnd;
        FSNode const* fsnode;
        int width;
        bool isSpecial:1;
        bool isHidden:1;
    };

    const unsigned NON_SPECIAL_MENU_ID_START = 100;

public:
    HWND hwnd() { return mHWnd; }
    /*HWND getAddrHWnd() { return mAddrHWnd; }
    HWND getGoHWnd() { return mGoHWnd; }
    HWND getRefreshHWnd() { return mRefreshHWnd; }*/

public:
    static NavBar* create(HINSTANCE hInstance, MainWnd* parentWnd);

    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK addrWndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK addrWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
 
    int getPreferredHeight();

    void recreateAddrButtons();
    void resizeAddrButtons();

private:
    AddrButton& addAddrButton();
    void calcAddrButtonWidth(AddrButton& button, std::wstring const& text);
    void initAddrButtons();

    void setAddrEditMode(bool val);

    void createSpecialPopupMenu();

    void initButtons();

private:
    HWND mHWnd;
    WNDPROC mOrigWndProc;
    WNDPROC mAddrOrigWndProc;

    HWND mAddrHWnd;
    HWND mGoHWnd;
    HWND mRefreshHWnd;

    HMENU mSpecialPopupMenuHandle;

    bool mIsEditMode;

    std::vector<AddrButton> mAddrButtons;
    unsigned mActiveAddrButtonsCount;
    int mAddrButtonsWidth;

    MainWnd* parentWnd;
};