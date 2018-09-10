#pragma once

#include <windows.h>

#include <string>

class InputDialog
{
public:
    InputDialog(std::wstring title, std::wstring promtText)
        : mTitle(title), mPromtText(promtText), mInitText(), mSelectAllOnInit(), mRes(false)
    {
    }

    void setInitText(std::wstring text) { mInitText = text; }

    void setSelectAllOnInit(bool val) { mSelectAllOnInit = val; }

    bool getRes() { return mRes; }

    std::wstring& getResText() { return mResText; }

    void show();

private:
    INT_PTR CALLBACK dlgProcInternal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK dlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

private:
    std::wstring mTitle;
    std::wstring mPromtText;
    std::wstring mInitText;
    std::wstring mResText;
    bool mSelectAllOnInit;
    bool mRes;
};