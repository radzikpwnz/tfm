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

#include "resource.h"
#include "state.h"
#include "core.h"
#include "stuff.h"
#include "env.h"
#include "toolbar.h"

#include "mainwnd.h"


// Main window class name
static const wchar_t MainWndClass[] = L"TFM_MainWnd";
// Main window initial title
static const wchar_t MainWndTitle[] = L"Tiny File Manager";

static const wchar_t InstanceProp[] = L"INSTANCE";


// About dialog procedure
INT_PTR CALLBACK
AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

            LOGFONT lf;

            lf = {};
            wcscpy_s(lf.lfFaceName, L"MS Shell Dlg");
            lf.lfHeight = 18;
            lf.lfWeight = FW_BOLD;
            lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
            HFONT hFontBold = CreateFontIndirect(&lf);
            SendMessage(GetDlgItem(hDlg, IDC_ABOUT_HEADER), WM_SETFONT, (WPARAM)hFontBold, 1);

            lf = {};
            wcscpy_s(lf.lfFaceName, L"MS Shell Dlg");
            lf.lfHeight = 12;
            lf.lfItalic = 1;
            lf.lfQuality = CLEARTYPE_NATURAL_QUALITY;
            HFONT hFontItalic = CreateFontIndirect(&lf);
            SendMessage(GetDlgItem(hDlg, IDC_ABOUT_VERSION), WM_SETFONT, (WPARAM)hFontItalic, 1);

            SetWindowText(GetDlgItem(hDlg, IDC_ABOUT_VERSION), ProgramVersion);

            return (INT_PTR)TRUE;
        }
        case WM_COMMAND:
        {
            if ( LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL )
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
            break;
        }
        case WM_NOTIFY:
        {
            NMHDR* hdr = (NMHDR*)lParam;
            if ( hdr->idFrom == IDC_ELTECHSCOM && hdr->code == NM_CLICK )
            {
                NMLINK* link = (NMLINK*)hdr;
                ShellExecute(NULL, L"open", link->item.szUrl, NULL, NULL, SW_SHOW);
            }
            break;
        }
    }

    return FALSE;
}

// On current path change handler
void
MainWnd::onCurPathChange()
{
    mNavBar->recreateAddrButtons();
    mNavBar->resizeAddrButtons();
    SetWindowText(mHWnd, GetCurPathFSNodes().back().getDisplayName().c_str());
}

// On content update handler
void
MainWnd::onContentUpdate()
{
    mContent->refreshContent();
}

// On tree element expanding handler
void
MainWnd::onTreeExpanding(HTREEITEM treeItem, FSNode const& node)
{
    mTreeView->refreshContentRec(treeItem, node);
}

// On tree refresh handler
void
MainWnd::onTreeRefresh()
{
    mTreeView->refreshContent();
}

// Set content view style (from menu)
void
MainWnd::setViewStyle(unsigned itemId) // menu item id
{
    static const unsigned viewItemIds[] = {
        ID_VIEW_LARGEICONS,
        ID_VIEW_SMALLICONS,
        ID_VIEW_LIST,
        ID_VIEW_DETAILS
    };

    MENUITEMINFO item;
    item.cbSize = sizeof(MENUITEMINFO);
    item.fMask = MIIM_STATE;
    
    for ( unsigned i = 0; i < 4; i++ )
    {
        unsigned id = viewItemIds[i];
        item.fState = ( itemId == id ) ? MFS_CHECKED : MFS_UNCHECKED;
        SetMenuItemInfo(GetMenu(mHWnd), id, FALSE, &item);
    }

    switch ( itemId )
    {
        case ID_VIEW_LARGEICONS:
            mContent->setViewStyle(ContentView::LARGE_ICON);
            break;
        case ID_VIEW_SMALLICONS:
            mContent->setViewStyle(ContentView::SMALL_ICON);
            break;
        case ID_VIEW_LIST:
            mContent->setViewStyle(ContentView::LIST);
            break;
        case ID_VIEW_DETAILS:
            mContent->setViewStyle(ContentView::DETAILS);
            break;
    }
}

// WM_COMMAND message handler
void
MainWnd::command(WPARAM wParam, LPARAM lParam)
{
    if ( lParam == 0 && HIWORD(wParam) == 0 )
    {
        // Menu
        switch ( LOWORD(wParam) )
        {
            case ID_FILE_NEWWINDOW:
            {
                InstanceManager::create(GetCurPath());
                break;
            }
            case ID_HELP_ABOUT:
                DialogBox(Globals.getHInstance(), MAKEINTRESOURCE(IDD_ABOUT), mHWnd, AboutDlgProc);
                break;
            case ID_FILE_CLOSE:
                DestroyWindow(mHWnd);
                break;
            case ID_VIEW_LARGEICONS:
            case ID_VIEW_SMALLICONS:
            case ID_VIEW_LIST:
            case ID_VIEW_DETAILS:
                setViewStyle(LOWORD(wParam));
                break;
        }
    } else if ( lParam == 0 && HIWORD(wParam) == 1 )
    {
        // Accelerator
    } else
    {
        // Control
        HWND hwnd = (HWND)lParam;
        if ( hwnd == mToolbar->hwnd() )
        {
            mToolbar->command(LOWORD(wParam));
        }
    }
}

// Resize handler
void
MainWnd::resize()
{
    RECT rect;
    GetClientRect(mHWnd, &rect);

    // Toolbar
    RECT toolbar_rect;
    RECT button_rect;
    SendMessage(mToolbar->hwnd(), TB_GETITEMRECT, 0, (LPARAM)&button_rect);
    SetWindowPos(mToolbar->hwnd(), NULL, 0, 0, rect.right, button_rect.bottom + 3, SWP_NOZORDER);
    GetWindowRectInParent(mToolbar->hwnd(), &toolbar_rect);

    // Status Bar
    RECT status_rect;
    SendMessage(mStatusBar->hwnd(), WM_SIZE, 0, 0);
    GetWindowRectInParent(mStatusBar->hwnd(), &status_rect);
    
    // Navigation Bar
    RECT navbar_rect;
    int navBarHeight = mNavBar->getPreferredHeight();
    SetWindowPos(mNavBar->hwnd(), NULL, 0, toolbar_rect.bottom, rect.right, navBarHeight, SWP_NOZORDER);
    GetWindowRectInParent(mNavBar->hwnd(), &navbar_rect);

    // Tree View
    RECT treeview_rect;
    int treeviewY = navbar_rect.bottom;
    int treeviewHeight = status_rect.top - navbar_rect.bottom;
    GetWindowRectInParent(mTreeView->hwnd(), &treeview_rect);
    SetWindowPos(mTreeView->hwnd(), NULL, 0, treeviewY, treeview_rect.right, treeviewHeight, SWP_NOZORDER);

    // Size bar
    RECT sizebar_rect;
    GetWindowRectInParent(mSizeBar->hwnd(), &sizebar_rect);
    SetWindowPos(mSizeBar->hwnd(), NULL, treeview_rect.right, treeviewY, sizebar_rect.right - sizebar_rect.left, treeviewHeight, SWP_NOZORDER);

    // Content
    int contentWidth = rect.right - sizebar_rect.right;
    SetWindowPos(mContent->hwnd(), NULL, sizebar_rect.right, treeviewY, contentWidth, treeviewHeight, SWP_NOZORDER);
}

// Window procedure (internal function)
LRESULT CALLBACK
MainWnd::wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch ( message )
    {
        case WM_SIZE:
        {
            resize();
            return 0;
        }
        case WM_COMMAND:
        {
            command(wParam, lParam);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_NOTIFY:
        {
            // Call control's WM_NOTIFY handler
            NMHDR* hdr = (NMHDR*)lParam;
            if ( mContent != nullptr && hdr->hwndFrom == mContent->hwnd() )
            {
                return mContent->notify(hdr);
            } else if ( mTreeView != nullptr && hdr->hwndFrom == mTreeView->hwnd() )
            {
                return mTreeView->notify(hdr);
            } else
            {
                return 0;
            }
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Window procedure
LRESULT CALLBACK
MainWnd::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MainWnd* wnd = (MainWnd*)GetProp(hWnd, InstanceProp);
    if ( wnd == nullptr )
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return wnd->wndProcInternal(hWnd, message, wParam, lParam);
}

// Register window class
bool
MainWnd::registerWindowClass()
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wndProc;
    wcex.hInstance = Globals.getHInstance();
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MAIN_MENU);
    wcex.lpszClassName = MainWndClass;
    wcex.hIcon = LoadIcon(Globals.getHInstance(), MAKEINTRESOURCE(IDI_MAIN));
    //wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Common initialization
bool
MainWnd::initCommon()
{
    return registerWindowClass();
}

// Create main window instance
MainWnd*
MainWnd::create(HINSTANCE hInstance)
{
    MainWnd* wnd = new MainWnd();

    wnd->mHWnd = CreateWindowEx(0, MainWndClass, MainWndTitle, WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_DESKTOP, nullptr, hInstance, nullptr);
    if ( wnd->mHWnd == NULL )
    {
        return nullptr;
    }

    SetProp(wnd->mHWnd, InstanceProp, wnd);

    wnd->mToolbar = Toolbar::create(hInstance, wnd);
    wnd->mNavBar = NavBar::create(hInstance, wnd);
    wnd->mTreeView = TreeView::create(hInstance, wnd);
    wnd->mContent = ContentView::create(hInstance, wnd);
    wnd->mStatusBar = StatusBar::create(hInstance, wnd);
    wnd->mSizeBar = SizeBar::create(hInstance, wnd);

    wnd->setViewStyle(ID_VIEW_DETAILS);

    RECT rect;
    GetClientRect(wnd->mHWnd, &rect);
    int treeViewWidth = std::min(rect.right / 5, 200l);
    SetWindowPos(wnd->mTreeView->hwnd(), NULL, 0, 0, treeViewWidth, 0, SWP_NOZORDER | SWP_NOMOVE);

    wnd->mSizeBar->setSizedControls(wnd->mTreeView->hwnd(), wnd->mContent->hwnd());
    SetWindowPos(wnd->mSizeBar->hwnd(), NULL, treeViewWidth, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    return wnd;
}