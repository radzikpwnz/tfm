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