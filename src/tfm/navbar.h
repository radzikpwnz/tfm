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
#include "mainwnd_fwd.h"

// Navigation bar
class NavBar
{
private:
    // Address (path) fragment button
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

        HWND hwnd;             // hwnd
        FSNode const* fsnode;  // FSNode
        int width;             // width
        bool isSpecial:1;      // special "More" button flag
        bool isHidden:1;       // hidden flag
    };

    // Start menu item id for common (non-special) address fragment buttons (when they hidden)
    const unsigned NON_SPECIAL_MENU_ID_START = 100;

public:
    static NavBar* create(HINSTANCE hInstance, MainWnd* parentWnd);

private:
    static LRESULT CALLBACK addrWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
    // Get hwnd
    HWND hwnd() { return mHWnd; }

    LRESULT CALLBACK addrWndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
 
    int getPreferredHeight();

    void recreateAddrButtons();
    void resizeAddrButtons();

private:
    AddrButton& addAddrButton();
    void calcAddrButtonWidth(AddrButton& button, std::wstring const& text);
    void initAddrButtons();

    void setAddrEditMode(bool val);

    void createMorePopupMenu();

    void initButtons();

private:
    HWND mHWnd;                            // hwnd
    MainWnd* mParentWnd;                   // parent window
    WNDPROC mOrigWndProc;                  // orig window procedure
    WNDPROC mAddrOrigWndProc;              // address window orig window procedure

    HWND mAddrHWnd;                        // addr window hwnd
    HWND mGoHWnd;                          // "Go" button hwnd
    HWND mRefreshHWnd;                     // "Refresh" button hwnd

    HMENU mMorePopupMenuHandle;            // "More" popup menu handle

    bool mIsEditMode;                      // address edit mode flag

    std::vector<AddrButton> mAddrButtons;  // address fragment button
    unsigned mActiveAddrButtonsCount;      // active address fragment buttons count
    int mAddrButtonsWidth;                 // total address fragment button width
};