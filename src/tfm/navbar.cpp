#include "common.h"

#include "resource.h"
#include "stuff.h"
#include "core.h"
#include "state.h"
#include "mainwnd.h"

#include "navbar.h"


static const wchar_t InstanceProp[] = L"INSTANCE";


// Window procedure (internal function)
LRESULT CALLBACK
NavBar::wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch ( message )
    {
        case WM_SIZE:
        {
            RECT rect;
            GetClientRect(mHWnd, &rect);

            RECT refresh_rect;
            RECT go_rect;
            GetWindowRectInParent(mRefreshHWnd, &refresh_rect);
            GetWindowRectInParent(mGoHWnd, &go_rect);
            int curNavX = rect.right - (4 + (refresh_rect.right - refresh_rect.left));
            int ctrlHeight = refresh_rect.bottom - refresh_rect.top;
            SetWindowPos(mRefreshHWnd, NULL, curNavX, 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
            curNavX -= (4 + (go_rect.right - go_rect.left));
            SetWindowPos(mGoHWnd, NULL, curNavX, 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
            SetWindowPos(mAddrHWnd, NULL, 4, 5, curNavX - 4 - 4, ctrlHeight - 6, SWP_NOZORDER);
            break;
        }
        case WM_COMMAND:
        {
            HWND hwnd = (HWND)lParam;
            if ( hwnd == mGoHWnd )
            {
                int len = GetWindowTextLength(mAddrHWnd);
                std::wstring path(len, 0);
                SendMessage(mAddrHWnd, WM_GETTEXT, len + 1, (LPARAM)&path[0]);
                path.resize(len);
                setAddrEditMode(false);
                SetCurPathFromCmdlineArg(path);
            } else if ( hwnd == mRefreshHWnd )
            {
                NavigateRefresh();
                //TreeFullRefresh();
            } else if ( hwnd == 0 )
            {
                MENUITEMINFO item;
                item.cbSize = sizeof(MENUITEMINFO);
                item.fMask =  MIIM_DATA;
                GetMenuItemInfo(mMorePopupMenuHandle, LOWORD(wParam), FALSE, &item);
                FSNode* node = (FSNode*)item.dwItemData;
                if ( LOWORD(wParam) < NON_SPECIAL_MENU_ID_START )
                {
                    NavigateToRootNode(node);
                } else
                {
                    NavigateUp(node);
                }
            }

            return 0;
        }
    }

    return mOrigWndProc(hWnd, message, wParam, lParam);
}

// Window procedure
LRESULT CALLBACK
NavBar::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    NavBar* navBar = (NavBar*)GetProp(hWnd, InstanceProp);
    return navBar->wndProcInternal(hWnd, message, wParam, lParam);
}

// Address window procedure (internal function)
LRESULT CALLBACK
NavBar::addrWndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch ( message )
    {
        case WM_KILLFOCUS:
        {
            if ( (HWND)wParam != mGoHWnd )
            {
                setAddrEditMode(false);
            }
            break;
        }
        case WM_COMMAND:
        {
            AddrButton& button = mAddrButtons[LOWORD(wParam)];
            if ( button.isSpecial )
            {
                createMorePopupMenu();
            } else
            {
                NavigateUp(button.fsnode);
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            if ( !mIsEditMode )
            {
                setAddrEditMode(true);
            }
            break;
        }
        case WM_SIZE:
        {
            resizeAddrButtons();
            break;
        }
    }

    return mAddrOrigWndProc(hWnd, message, wParam, lParam);
}

// Address window procedure
LRESULT CALLBACK
NavBar::addrWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    NavBar* navBar = (NavBar*)GetProp(GetParent(hWnd), InstanceProp);
    return navBar->addrWndProcInternal(hWnd, message, wParam, lParam);
}

// Get preferres navigation bar height
int
NavBar::getPreferredHeight()
{
    RECT refresh_rect;
    GetWindowRectInParent(mRefreshHWnd, &refresh_rect);
    return refresh_rect.bottom - refresh_rect.top + 6;
}

// Create new address fragment button
NavBar::AddrButton&
NavBar::addAddrButton()
{
    int index = mAddrButtons.size();

    AddrButton button;
    button.hwnd = CreateWindowEx(0, WC_BUTTON, NULL,
                                 WS_CHILD | WS_CLIPSIBLINGS,
                                 0, 0, 0, 0,
                                 mAddrHWnd, (HMENU)index, Globals.getHInstance(), NULL);

    SendMessage(button.hwnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);

    mAddrButtons.push_back(button);
    return mAddrButtons[mAddrButtons.size() - 1];
}

// Calculate address fragment button width by it's text
void
NavBar::calcAddrButtonWidth(AddrButton& button, std::wstring const& text)
{
    HDC hDC = GetDC(button.hwnd);
    HGDIOBJ prevFont = SelectObject(hDC, (HFONT)SendMessage(button.hwnd, WM_GETFONT, 0, 0));
    SIZE size;
    GetTextExtentPoint32(hDC, text.c_str(), text.length(), &size);
    button.width = size.cx + 8;
    SelectObject(hDC, prevFont);
    ReleaseDC(button.hwnd, hDC);
}

// Init address fragment buttons
void
NavBar::initAddrButtons()
{
    mAddrButtons.clear();
    
    AddrButton& specialButton = addAddrButton();
    specialButton.isSpecial = true;
    specialButton.isHidden = false;

    SetWindowLongPtr(specialButton.hwnd, GWL_STYLE, GetWindowLongPtr(specialButton.hwnd, GWL_STYLE) | BS_BITMAP);
    COLORMAP colorMap;
    colorMap.from = RGB(255, 0, 255);
    colorMap.to = GetSysColor(COLOR_BTNFACE);
    HBITMAP hBmp = CreateMappedBitmap(GetModuleHandle(NULL), IDB_BTN_NAV_SPECIAL, 0, &colorMap, 1);
    SendMessageW(specialButton.hwnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);

    specialButton.width = 20;

    mAddrButtonsWidth = specialButton.width;
    mActiveAddrButtonsCount = 1;

    resizeAddrButtons();
}

// Recreate address fragment buttons (on address change)
void
NavBar::recreateAddrButtons()
{
    std::vector<FSNode> const& nodes = GetCurPathFSNodes();

    if ( mAddrButtons.size() < nodes.size() + 1 )
    {
        for ( unsigned i = mAddrButtons.size(); i < nodes.size() + 1; i++ )
        {
            addAddrButton();
        }
    }

    mActiveAddrButtonsCount = nodes.size() + 1;

    for ( unsigned i = mActiveAddrButtonsCount; i < mAddrButtons.size(); i++ )
    {
        ShowWindow(mAddrButtons[i].hwnd, SW_HIDE);
        mAddrButtons[i].isHidden = true;
    }
   

    for ( unsigned i = 1; i < nodes.size() + 1; i++ )
    {
        mAddrButtons[i].fsnode = &nodes[i - 1];
        std::wstring text = mAddrButtons[i].fsnode->getDisplayName() + L" >";
        SendMessage(mAddrButtons[i].hwnd, WM_SETTEXT, 0, (LPARAM)text.c_str());
        calcAddrButtonWidth(mAddrButtons[i], text);
    }

    mAddrButtonsWidth = 0;
    for ( unsigned i = 0; i < mActiveAddrButtonsCount; i++ )
    {
        mAddrButtonsWidth += mAddrButtons[i].width;
    }
}

// Resize address fragment buttons (on navigation bar resize)
void
NavBar::resizeAddrButtons()
{
    RECT addrRect;
    GetClientRect(mAddrHWnd, &addrRect);

    int maxWidth = addrRect.right - 50;

    mAddrButtons[mActiveAddrButtonsCount - 1].isHidden = false;
    int curWidth = mAddrButtons[0].width + mAddrButtons[mActiveAddrButtonsCount - 1].width;
    int i = mActiveAddrButtonsCount - 2;
    while ( i > 0 )
    {
        AddrButton& button = mAddrButtons[i];
        curWidth += button.width;
        if ( curWidth < maxWidth )
        {
            button.isHidden = false;
        } else
        {
            break;
        }
        i--;
    }

    for ( int j = 1; j <= i; j++ )
    {
        mAddrButtons[j].isHidden = true;
        
    }

    int curX = 1;
    for ( unsigned j = 0; j < mActiveAddrButtonsCount; j++ )
    {
        AddrButton& button = mAddrButtons[j];
        if ( button.isHidden )
        {
            ShowWindow(button.hwnd, SW_HIDE);
        } else
        {
            SetWindowPos(button.hwnd, NULL, curX, 1, button.width, addrRect.bottom - 2, SWP_NOZORDER);
            ShowWindow(button.hwnd, SW_SHOW);
            curX += button.width;
        }
    }
}

// Enter/exit address edit mode
void
NavBar::setAddrEditMode(bool val)
{
    for ( unsigned i = 0; i < mActiveAddrButtonsCount; i++ )
    {
        AddrButton& button = mAddrButtons[i];
        if ( !button.isHidden )
        {
            ShowWindow(button.hwnd, val ? SW_HIDE : SW_SHOW);
        }
    }

    SendMessage(mAddrHWnd, EM_SETREADONLY, val ? FALSE : TRUE, 0);

    if ( val )
    {
        SendMessage(mAddrHWnd, WM_SETTEXT, 0, (LPARAM)GetCurPath().c_str());
        PostMessage(mAddrHWnd, EM_SETSEL, 0, -1);
    } else
    {
        SendMessage(mAddrHWnd, WM_SETTEXT, 0, (LPARAM)L"");
    }

    mIsEditMode = val;

    if ( !mIsEditMode )
    {
        resizeAddrButtons();
    }
}

// Create "More" popup menu
void
NavBar::createMorePopupMenu()
{
    HMENU menu = CreatePopupMenu();

    MENUITEMINFO item = {};
    item.cbSize = sizeof(MENUITEMINFO);
    item.fMask = MIIM_TYPE | MIIM_DATA | MIIM_ID;
    item.fType = MFT_STRING;

    unsigned itemId = NON_SPECIAL_MENU_ID_START;

    for ( unsigned i = mActiveAddrButtonsCount - 1; i > 0; i-- )
    {
        AddrButton& button = mAddrButtons[i];
        FSNode const* node = button.fsnode;

        if ( !button.isHidden )
        {
            continue;
        }

        item.dwTypeData = const_cast<wchar_t*>(&node->getDisplayName()[0]);
        item.cch = node->getDisplayName().length();
        item.wID = itemId++;
        item.dwItemData = (ULONG_PTR)node;

        InsertMenuItem(menu, (unsigned)-1, TRUE, &item);
    }

    item.fType = MFT_SEPARATOR;
    InsertMenuItem(menu, (unsigned)-1, TRUE, &item);
    item.fType = MFT_STRING;

    itemId = 1;

    std::vector<FSNode> const& rootNodes = GetRootFSNodes();
    for ( unsigned i = 0; i < rootNodes.size(); i++ )
    {
        FSNode const* node = &rootNodes[i];

        item.dwTypeData = const_cast<wchar_t*>(&node->getDisplayName()[0]);
        item.cch = node->getDisplayName().length();
        item.wID = itemId++;
        item.dwItemData = (ULONG_PTR)node;

        InsertMenuItem(menu, (unsigned)-1, TRUE, &item);
    }

    mMorePopupMenuHandle = menu;
    POINT cursor;
    GetCursorPos(&cursor);
    TrackPopupMenu(menu, 0, cursor.x, cursor.y, 0, mHWnd, nullptr);
}

// Init "Go" and "Refresh" buttons
void
NavBar::initButtons()
{
    COLORMAP colorMap;
    colorMap.from = RGB(255, 0, 255);
    colorMap.to = GetSysColor(COLOR_BTNFACE);
    HBITMAP hBmpGo = CreateMappedBitmap(GetModuleHandle(NULL), IDB_BTN_GO, 0, &colorMap, 1);
    HBITMAP hBmpRefresh = CreateMappedBitmap(GetModuleHandle(NULL), IDB_BTN_REFRESH, 0, &colorMap, 1);

    SendMessage(mGoHWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpGo);
    SetWindowPos(mGoHWnd, NULL, 0, 0, 30, 30, SWP_NOZORDER | SWP_NOMOVE);

    SendMessage(mRefreshHWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpRefresh);
    SetWindowPos(mRefreshHWnd, NULL, 0, 0, 30, 30, SWP_NOZORDER | SWP_NOMOVE);
}

// Create navigation bar instance
NavBar*
NavBar::create(HINSTANCE hInstance, MainWnd* parentWnd)
{
    NavBar* navBar = new NavBar();

    navBar->mHWnd = CreateWindowEx(0, WC_STATIC, NULL,
                                   WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER,
                                   0, 0, 0, 0,
                                   parentWnd->hwnd(), (HMENU)NULL, hInstance, NULL);

    if ( navBar->mHWnd == NULL )
    {
        return nullptr;
    }

    navBar->mAddrHWnd = CreateWindowEx(0, WC_EDIT, NULL,
                                       WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER | ES_AUTOHSCROLL,
                                       0, 0, 0, 0,
                                       navBar->mHWnd, (HMENU)NULL, hInstance, NULL);
    SendMessage(navBar->mAddrHWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);

    navBar->mGoHWnd = CreateWindowEx(0, WC_BUTTON, NULL,
                                     WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_BITMAP,
                                     0, 0, 30, 30,
                                     navBar->mHWnd, (HMENU)NULL, hInstance, NULL);

    navBar->mRefreshHWnd = CreateWindowEx(0, WC_BUTTON, NULL,
                                          WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | BS_BITMAP,
                                          0, 0, 30, 30,
                                          navBar->mHWnd, (HMENU)NULL, hInstance, NULL);

    navBar->initButtons();

    navBar->mOrigWndProc = (WNDPROC)SetWindowLongPtr(navBar->mHWnd, GWLP_WNDPROC, (LONG_PTR)wndProc);
    SetProp(navBar->mHWnd, InstanceProp, navBar);

    navBar->mAddrOrigWndProc = (WNDPROC)SetWindowLongPtr(navBar->mAddrHWnd, GWLP_WNDPROC, (LONG_PTR)addrWndProc);

    navBar->initAddrButtons();
    navBar->setAddrEditMode(false);

    navBar->parentWnd = parentWnd;
    UpdateWindow(navBar->mHWnd);
    return navBar;
}