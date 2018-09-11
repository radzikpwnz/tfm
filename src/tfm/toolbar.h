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