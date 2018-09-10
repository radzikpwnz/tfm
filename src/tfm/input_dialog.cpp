#include "common.h"

#include "input_dialog.h"


static const wchar_t InstanceProp[] = L"INSTANCE";


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


void
InputDialog::show()
{
    DialogBoxParam(Globals.getHInstance(), MAKEINTRESOURCE(IDD_INPUT), InstanceManager::getCurrent()->getMainWnd()->hwnd(), dlgProc, (LPARAM)this);
}