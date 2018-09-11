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

#include "common.h"

#include "core.h"
#include "resource.h"

#include "input_dialog.h"


static const wchar_t InstanceProp[] = L"INSTANCE";


// Dialog procedure (internal function)
INT_PTR CALLBACK
InputDialog::dlgProcInternal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch ( message )
    {
        case WM_INITDIALOG:
        {
            RECT rect, rect1;
            GetWindowRect(GetParent(hDlg), &rect);
            GetClientRect(hDlg, &rect1);
            SetWindowPos(hDlg, NULL, (rect.right + rect.left) / 2 - (rect1.right - rect1.left) / 2,
                         (rect.bottom + rect.top) / 2 - (rect1.bottom - rect1.top) / 2,
                         0, 0, SWP_NOZORDER | SWP_NOSIZE);

            SetWindowText(hDlg, mTitle.c_str());
            SetWindowText(GetDlgItem(hDlg, IDC_PROMT), mPromtText.c_str());
            SetWindowText(GetDlgItem(hDlg, IDC_TEXT), mInitText.c_str());

            SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hDlg, IDC_TEXT), TRUE);
            if ( mSelectAllOnInit )
            {
                SendMessage(GetDlgItem(hDlg, IDC_TEXT), EM_SETSEL, 0, -1);
            }

            return (INT_PTR)FALSE;
        }
        case WM_COMMAND:
        {
            if ( LOWORD(wParam) == IDOK )
            {
                int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_TEXT));
                mResText.resize(len);
                SendMessage(GetDlgItem(hDlg, IDC_TEXT), WM_GETTEXT, len + 1, (LPARAM)&mResText[0]);

                mRes = true;
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            } else if ( LOWORD(wParam) == IDCANCEL )
            {
                mRes = false;
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
        }
    }

    return FALSE;
}

// Dialog procedure
INT_PTR CALLBACK
InputDialog::dlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    if ( message == WM_INITDIALOG )
    {
        SetProp(hDlg, InstanceProp, (HANDLE)lParam);
    }

    InputDialog* dlg = (InputDialog*)GetProp(hDlg, InstanceProp);
    return dlg->dlgProcInternal(hDlg, message, wParam, lParam);
}

// Show input dialog
void
InputDialog::show()
{
    DialogBoxParam(Globals.getHInstance(), MAKEINTRESOURCE(IDD_INPUT), InstanceManager::getCurrent()->getMainWnd()->hwnd(), dlgProc, (LPARAM)this);
}