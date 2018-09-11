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

// Dialog for requesting user text input
class InputDialog
{
public:
    InputDialog(std::wstring title, std::wstring promtText)
        : mTitle(title), mPromtText(promtText), mInitText(), mSelectAllOnInit(), mRes(false)
    {
    }

    // Set initial text
    void setInitText(std::wstring text) { mInitText = text; }

    // Set select all text on init flag
    void setSelectAllOnInit(bool val) { mSelectAllOnInit = val; }

    // Get result (ok/cancel)
    bool getRes() { return mRes; }

    // Get result text
    std::wstring& getResText() { return mResText; }

    void show();

private:
    INT_PTR CALLBACK dlgProcInternal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK dlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

private:
    std::wstring mTitle;      // title
    std::wstring mPromtText;  // promt text
    std::wstring mInitText;   // initial text
    std::wstring mResText;    // [out] result text
    bool mSelectAllOnInit;    // select all text on init flag
    bool mRes;                // [out] result
};