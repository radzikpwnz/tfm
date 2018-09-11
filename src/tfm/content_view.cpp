#include "common.h"

#include <shlwapi.h>

#include "fsnode.h"
#include "state.h"
#include "env.h"
#include "mainwnd.h"
#include "actions.h"
#include "input_dialog.h"

#include "content_view.h"


// Context menu elements
ContentView::CtxMenuElem ContentView::ctxOpen =         { L"Open",             &ContentView::actOpen };
ContentView::CtxMenuElem ContentView::ctxCut =          { L"Cut",              &ContentView::actCut };
ContentView::CtxMenuElem ContentView::ctxCopy =         { L"Copy",             &ContentView::actCopy };
ContentView::CtxMenuElem ContentView::ctxCreateLink =   { L"Create link",      &ContentView::actCreateLink };
ContentView::CtxMenuElem ContentView::ctxDelete =       { L"Delete",           &ContentView::actDelete };
ContentView::CtxMenuElem ContentView::ctxRename =       { L"Rename",           &ContentView::actRename };
ContentView::CtxMenuElem ContentView::ctxPaste =        { L"Paste",            &ContentView::actPaste };
ContentView::CtxMenuElem ContentView::ctxPasteAsLink =  { L"Paste as link",    &ContentView::actPasteAsLink };
ContentView::CtxMenuElem ContentView::ctxNewFolder =    { L"New folder",       &ContentView::actNewFolder };

static const wchar_t InstanceProp[] = L"INSTANCE";


// Window procedure (internal function)
LRESULT CALLBACK
ContentView::wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch ( message )
    {
        case WM_COMMAND:
        {
            HWND hwnd = (HWND)lParam;
            if ( hwnd == 0 )
            {
                // Context menu element click
                MENUITEMINFO item;
                item.cbSize = sizeof(MENUITEMINFO);
                item.fMask = MIIM_DATA;
                GetMenuItemInfo(mCtxMenuHandle, LOWORD(wParam), FALSE, &item);
                CtxMenuElem* elem = (CtxMenuElem*)item.dwItemData;
                (this->*(elem->proc))();
            }

            break;
        }
    }

    return mOrigWndProc(hWnd, message, wParam, lParam);
}

// Window procedure
LRESULT CALLBACK
ContentView::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ContentView* contentView = (ContentView*)GetProp(hWnd, InstanceProp);
    return contentView->wndProcInternal(hWnd, message, wParam, lParam);
}

// Set content view style
void
ContentView::setViewStyle(ViewStyle style)
{
    LONG_PTR wndstyle = GetWindowLongPtr(mHWnd, GWL_STYLE);
    wndstyle &= ~LVS_TYPEMASK;

    switch ( style )
    {
        case LARGE_ICON:
            wndstyle |= LVS_ICON;
            break;
        case SMALL_ICON:
            wndstyle |= LVS_SMALLICON;
            break;
        case LIST:
            wndstyle |= LVS_LIST;
            break;
        case DETAILS:
            wndstyle |= LVS_REPORT;
            break;
    }

    SetWindowLongPtr(mHWnd, GWL_STYLE, wndstyle);

    mViewStyle = style;
    refreshContent();
}

// Get selected items
uint32_t
ContentView::getSelectedItems(std::vector<SelItem>& resList) // destination list
{
    uint32_t typeMask = 0;
    resList.clear();

    int pos = ListView_GetNextItem(mHWnd, -1, LVNI_SELECTED);
    while ( pos != -1 )
    {
        LVITEM item;
        item.iItem = pos;
        item.mask = LVIF_PARAM;
        ListView_GetItem(mHWnd, &item);

        FSNode* fsnode = (FSNode*)item.lParam;
        typeMask |= 1 << fsnode->getType();
        resList.push_back({ pos, fsnode });

        pos = ListView_GetNextItem(mHWnd, pos, LVNI_SELECTED);
    }

    return typeMask;
}

/*static int CALLBACK
NodesCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    FSNode* node1 = (FSNode*)lParam1;
    FSNode* node2 = (FSNode*)lParam2;
    if (  )
}*/

// Clean and rebuild all content
void
ContentView::refreshContent()
{
    ListView_DeleteAllItems(mHWnd);

    std::vector<FSNode> const& nodes = GetCurContentFSNodes();

    LVITEM item;

    //item.pszText = LPSTR_TEXTCALLBACK;
    item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
    item.stateMask = 0;
    item.iSubItem = 0;
    item.state = 0;

    for ( unsigned i = 0; i < nodes.size(); i++ )
    {
        FSNode const& node = nodes[i];

        SHFILEINFO sfi;
        fs::path filePath = GetCurPath() / node.getRelPath();
        if ( mViewStyle == LARGE_ICON )
        {
            HIMAGELIST himl = (HIMAGELIST)SHGetFileInfo(filePath.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_ICON);
            ListView_SetImageList(mHWnd, himl, LVSIL_NORMAL);
        } else
        {
            HIMAGELIST himl = (HIMAGELIST)SHGetFileInfo(filePath.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
            ListView_SetImageList(mHWnd, himl, LVSIL_SMALL);
        }

        item.pszText = const_cast<wchar_t*>(&node.getDisplayName()[0]);

        item.iItem = i;
        item.iSubItem = 0;
        item.iImage = sfi.iIcon;
        item.lParam = (LPARAM)&node;
        ListView_InsertItem(mHWnd, &item);

        if ( node.getType() == FSNode::FILE )
        {
            std::error_code err;
            uint64_t fileSize = fs::file_size(filePath, err);
            if ( !err )
            {
                wchar_t sizeBuf[16];
                StrFormatByteSizeW(fileSize, sizeBuf, 16);
                ListView_SetItemText(mHWnd, i, 1, sizeBuf);
            }
        }

        SHGetFileInfo(filePath.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_TYPENAME);
        ListView_SetItemText(mHWnd, i, 2, sfi.szTypeName);

    }

    //ListView_SortItems(mHWnd, NodesCompareFunc, 0);
}

// Insert element in current context menu
void
ContentView::insertCtxMenuElem(unsigned id,       // element id
                               CtxMenuElem* elem, // element descr
                               bool insertSep)    // insert separator after element
{
    MENUITEMINFO item = {};
    item.cbSize = sizeof(MENUITEMINFO);
    item.fMask = MIIM_TYPE | MIIM_DATA | MIIM_ID;
    item.fType = MFT_STRING;

    item.dwTypeData = &elem->text[0];
    item.cch = elem->text.length();
    item.wID = id;
    item.dwItemData = (ULONG_PTR)elem;

    InsertMenuItem(mCtxMenuHandle, (unsigned)-1, TRUE, &item);

    if ( insertSep )
    {
        item.fType = MFT_SEPARATOR;
        InsertMenuItem(mCtxMenuHandle, (unsigned)-1, TRUE, &item);
    }
}

// Create context menu
void
ContentView::createContextMenu(ContextMenuType type)
{
    HMENU hMenu = CreatePopupMenu();
    mCtxMenuHandle = hMenu;

    unsigned id = 0;

    if ( type == MENU_SINGLE || type == MENU_MULTIPLE )
    {
        if ( type == MENU_SINGLE )
        {
            insertCtxMenuElem(id++, &ctxOpen, true);
        }
        insertCtxMenuElem(id++, &ctxCut);
        insertCtxMenuElem(id++, &ctxCopy, true);
        insertCtxMenuElem(id++, &ctxCreateLink);
        insertCtxMenuElem(id++, &ctxDelete);
        if ( type == MENU_SINGLE )
        {
            insertCtxMenuElem(id++, &ctxRename);
        }
    } else
    {
        insertCtxMenuElem(id++, &ctxPaste);
        insertCtxMenuElem(id++, &ctxPasteAsLink);
        insertCtxMenuElem(id++, &ctxNewFolder);
    }

    POINT cursor;
    GetCursorPos(&cursor);
    TrackPopupMenu(hMenu, 0, cursor.x, cursor.y, 0, mHWnd, nullptr);
}

// WM_NOTIFY message handler
LRESULT
ContentView::notify(NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
        case LVN_GETDISPINFO:
        {
            NMLVDISPINFO* info = (NMLVDISPINFO*)nmhdr;
            FSNode* fsnode = (FSNode*)info->item.lParam;

            switch ( info->item.iSubItem )
            {
                case 0:
                    info->item.pszText = const_cast<wchar_t*>(&fsnode->getDisplayName()[0]);
                    break;
                case 1:
                    info->item.pszText = L"";
                    break;
                case 2:
                    info->item.pszText = L"";
                    break;
                case 3:
                    info->item.pszText = L"";
                    break;
                default:
                    break;
            }
            break;
        }
        case NM_DBLCLK:
        {
            // Double click
            NMITEMACTIVATE* ia = (NMITEMACTIVATE*)nmhdr;

            if ( ia->iItem == -1 || ia->iSubItem != 0)
            {
                break;
            }

            LVITEM item;
            item.iItem = ia->iItem;
            item.iSubItem = ia->iSubItem;
            item.mask = LVIF_PARAM;
            ListView_GetItem(mHWnd, &item);

            FSNode* fsnode = (FSNode*)item.lParam;
            if ( fsnode->getType() == FSNode::DIR || fsnode->getType() == FSNode::LOGICAL_DRIVE )
            {
                NavigateDown(fsnode->getRelPath());
            } else if ( fsnode->getType() == FSNode::FILE )
            {
                ShellExecute(mParentWnd->hwnd(), L"open", &GetFSNodeFullPath(*fsnode).wstring()[0], nullptr, &GetCurPath().wstring()[0], SW_SHOW);
            }
            break;
        }
        case NM_RCLICK:
        {
            // Right click
            NMITEMACTIVATE* ia = (NMITEMACTIVATE*)nmhdr;

            if ( ia->iItem != -1 && ia->iSubItem == 0 )
            {
                // Item(s) right click
                uint32_t typeMask = getSelectedItems(mSelItems);

                if ( (typeMask & ~(1 << FSNode::DIR) & ~(1 << FSNode::FILE)) != 0 )
                {
                    break;
                }

                createContextMenu(mSelItems.size() == 1 ? MENU_SINGLE : MENU_MULTIPLE);
            } else
            {
                // Empty space right click
                createContextMenu(MENU_EMPTY);
            }
    
            break;
        }
    }
    
    return 0;
}

// "Open" action
void
ContentView::actOpen()
{
    if ( mSelItems.size() != 1 )
    {
        return;
    }

    FSNode* fsnode = mSelItems[0].fsnode;

    if ( fsnode->getType() == FSNode::DIR )
    {
        NavigateDown(fsnode->getRelPath());
        return;
    }

    ShellExecute(mParentWnd->hwnd(), L"open", &GetFSNodeFullPath(*fsnode).wstring()[0], nullptr, &GetCurPath().wstring()[0], SW_SHOW);
}

// "Cut" action
void
ContentView::actCut()
{
    ClearClipboard();
    std::vector<fs::path>& clipboard = GetClipboard();

    for ( unsigned i = 0; i < mSelItems.size(); i++ )
    {
        clipboard.push_back(GetFSNodeFullPath(*mSelItems[i].fsnode));
    }

    SetClipboardIsCut(true);
}

// "Copy" action
void
ContentView::actCopy()
{
    ClearClipboard();
    std::vector<fs::path>& clipboard = GetClipboard();

    for ( unsigned i = 0; i < mSelItems.size(); i++ )
    {
        clipboard.push_back(GetFSNodeFullPath(*mSelItems[i].fsnode));
    }

    SetClipboardIsCut(false);
}

// "Create link" action
void
ContentView::actCreateLink()
{
    std::vector<fs::path> srcPaths;
    fs::path dstPath = GetDesktopPath();

    for ( unsigned i = 0; i < mSelItems.size(); i++ )
    {
        srcPaths.push_back(GetFSNodeFullPath(*mSelItems[i].fsnode));
    }

    CreateShortcuts(srcPaths, dstPath);
}

// "Delete" action
void
ContentView::actDelete()
{
    std::vector<fs::path> srcPaths;

    for ( unsigned i = 0; i < mSelItems.size(); i++ )
    {
        srcPaths.push_back(GetFSNodeFullPath(*mSelItems[i].fsnode));
    }

    DeleteFiles(srcPaths);
    NavigateRefresh();
    //TreeFullRefresh();
}

// "Rename" action
void
ContentView::actRename()
{
    if ( mSelItems.size() != 1 )
    {
        return;
    }

    FSNode* fsnode = mSelItems[0].fsnode;
    if ( fsnode->getType() != FSNode::DIR && fsnode->getType() != FSNode::FILE )
    {
        return;
    }

    InputDialog dialog(L"Rename", L"Enter new name:");
    dialog.setInitText(fsnode->getDisplayName());
    dialog.setSelectAllOnInit(true);
    dialog.show();
    if ( !dialog.getRes() || dialog.getResText().empty() )
    {
        return;
    }

    fs::path oldPath = GetFSNodeFullPath(*fsnode);
    fs::path newPath = oldPath;
    newPath.replace_filename(dialog.getResText());

    fs::rename(oldPath, newPath);

    NavigateRefresh();
}

// "Paste" action
void
ContentView::actPaste()
{
    PasteFiles(GetCurPath());
    NavigateRefresh();
}

// "Paste as link" action
void
ContentView::actPasteAsLink()
{
    CreateShortcuts(GetClipboard(), GetCurPath());
    NavigateRefresh();
}

// "New folder" action
void
ContentView::actNewFolder()
{
    fs::path curPath = GetCurPath();
    if ( curPath.empty() )
    {
        return;
    }

    InputDialog dialog(L"New folder", L"Enter folder name:");
    dialog.setInitText(L"");
    dialog.show();
    if ( !dialog.getRes() || dialog.getResText().empty() )
    {
        return;
    }

    fs::path dirPath = GetCurPath() / dialog.getResText();
    
    if ( IsPathExists(dirPath) )
    {
        return;
    }

    std::error_code err;
    fs::create_directory(dirPath, err);

    NavigateRefresh();
}


// Insert columns for DETAILS view style
void
ContentView::insertColumns()
{
    LVCOLUMN column = {};
    column.mask = LVCF_WIDTH | LVCF_TEXT;
    column.cx = 220;
    column.pszText = L"File";
    ListView_InsertColumn(mHWnd, 0, &column);
    column.cx = 70;
    column.pszText = L"Size";
    ListView_InsertColumn(mHWnd, 1, &column);
    column.cx = 130;
    column.pszText = L"Type";
    ListView_InsertColumn(mHWnd, 2, &column);
    /*column.cx = 120;
    column.pszText = L"Modified";
    ListView_InsertColumn(mHWnd, 3, &column);*/
}

// Create content view instance
ContentView*
ContentView::create(HINSTANCE hInstance, MainWnd* parentWnd)
{
    ContentView* contentView = new ContentView();

    contentView->mHWnd = CreateWindowEx(0, WC_LISTVIEW, NULL,
                                        WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER | LVS_REPORT | LVS_SHAREIMAGELISTS,
                                        0, 0, 0, 0,
                                        parentWnd->hwnd(), (HMENU)NULL, hInstance, NULL);

    if ( contentView->mHWnd == NULL )
    {
        return nullptr;
    }

    contentView->insertColumns();

    //DWORD exStyle = ListView_GetExtendedListViewStyle(contentView->mHWnd);
    //exStyle |= LVS_EX_FULLROWSELECT/*LVS_EX_DOUBLEBUFFER | LVS_EX_BORDERSELECT*/;
    //ListView_SetExtendedListViewStyle(contentView->mHWnd, exStyle);

    contentView->mOrigWndProc = (WNDPROC)SetWindowLongPtr(contentView->mHWnd, GWLP_WNDPROC, (LONG_PTR)wndProc);
    SetProp(contentView->mHWnd, InstanceProp, contentView);

    contentView->mParentWnd = parentWnd;
    UpdateWindow(contentView->mHWnd);
    return contentView;
}
