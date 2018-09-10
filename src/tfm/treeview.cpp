#include <windows.h>
#include <shlobj.h>

#include "common.h"

#include "mainwnd.h"

#include "treeview.h"


void
TreeView::refreshContentRec(HTREEITEM parentItem, FSNode const& parentNode)
{
    HTREEITEM child = TreeView_GetChild(mHWnd, parentItem);
    while ( child != NULL )
    {
        HTREEITEM toRemove = child;
        child = TreeView_GetNextSibling(mHWnd, child);
        TreeView_DeleteItem(mHWnd, toRemove);
    }

    TVINSERTSTRUCT is;
    is.hParent = parentItem;
    is.hInsertAfter = TVI_LAST;

    is.itemex.mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_TEXT | TVIF_STATE;

    fs::path parentPath = GetFSNodeFullPath(parentNode);

    std::vector<FSNode> const& childNodes = parentNode.getChildsList();
    for ( unsigned i = 0; i < childNodes.size(); i++ )
    {
        FSNode const& node = childNodes[i];

        SHFILEINFO sfi;
        fs::path filePath = parentPath / node.getRelPath();
        HIMAGELIST himl = (HIMAGELIST)SHGetFileInfo(filePath.c_str(), 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
        TreeView_SetImageList(mHWnd, himl, TVSIL_NORMAL);

        is.itemex.cChildren = node.hasChildFolders() ? 1 : 0;
        is.itemex.state = node.getChildsList().size() != 0 ? TVIS_EXPANDED : 0;
        is.itemex.stateMask = TVIS_EXPANDED;
        is.itemex.pszText = const_cast<wchar_t*>(&node.getDisplayName()[0]);
        is.itemex.cchTextMax = node.getDisplayName().length();
        is.itemex.iImage = sfi.iIcon;
        is.itemex.iSelectedImage = sfi.iIcon;
        is.itemex.lParam = (LPARAM)&node;

        HTREEITEM handle = TreeView_InsertItem(mHWnd, &is);
        refreshContentRec(handle, node);
    }
}

void
TreeView::refreshContent()
{
    TreeView_DeleteAllItems(mHWnd);

    TVINSERTSTRUCT is;
    is.hParent = NULL;
    is.hInsertAfter = TVI_ROOT;

    is.itemex.mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_TEXT | TVIF_STATE;

    std::vector<FSNode> const& rootNodes = GetFSNodesTree();

    for ( unsigned i = 0; i < rootNodes.size(); i++ )
    {
        FSNode const& node = rootNodes[i];

        ITEMIDLIST* pidl = nullptr;
        switch ( node.getSpecialType() )
        {
            case FSNode::DESKTOP:
                SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl);
                break;
            case FSNode::MY_DOCUMENTS:
                SHGetSpecialFolderLocation(NULL, CSIDL_MYDOCUMENTS, &pidl);
                break;
            case FSNode::MY_COMPUTER:   
                SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidl);
                break;
            default:
                assert(false);
                break;
        }
        SHFILEINFO sfi;
        HIMAGELIST himl = (HIMAGELIST)SHGetFileInfo((LPCWSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_PIDL);
        CoTaskMemFree(pidl);
        TreeView_SetImageList(mHWnd, himl, TVSIL_NORMAL);

        is.itemex.cChildren = node.hasChildFolders() ? 1 : 0;
        is.itemex.state = node.getChildsList().size() != 0 ? TVIS_EXPANDED : 0;
        is.itemex.stateMask = TVIS_EXPANDED;
        is.itemex.pszText = const_cast<wchar_t*>(&node.getDisplayName()[0]);
        is.itemex.cchTextMax = node.getDisplayName().length();
        is.itemex.iImage = sfi.iIcon;
        is.itemex.iSelectedImage = sfi.iIcon;
        is.itemex.lParam = (LPARAM)&node;

        HTREEITEM handle = TreeView_InsertItem(mHWnd, &is);
        refreshContentRec(handle, node);
    }

}

TreeView*
TreeView::create(HINSTANCE hInstance, MainWnd* parentWnd)
{
    TreeView* treeView = new TreeView();

    treeView->mHWnd = CreateWindowEx(0, WC_TREEVIEW, NULL,
                                     WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
                                     0, 0, 0, 0,
                                     parentWnd->hwnd(), (HMENU)NULL, hInstance, NULL);

    if ( treeView->mHWnd == NULL )
    {
        return nullptr;
    }

    treeView->refreshContent();

    treeView->parentWnd = parentWnd;
    UpdateWindow(treeView->mHWnd);
    return treeView;
}

LRESULT
TreeView::notify(NMHDR* nmhdr)
{
    switch ( nmhdr->code )
    {
        case TVN_ITEMEXPANDING:
        {
            NMTREEVIEW* nmt = (NMTREEVIEW*)nmhdr;
            FSNode* fsnode = (FSNode*)nmt->itemNew.lParam;
            if ( nmt->action == TVE_EXPAND )
            {
                TreeExpanding(nmt->itemNew.hItem, fsnode);
            }
            return FALSE;
        }
        case TVN_ITEMEXPANDED:
        {
            NMTREEVIEW* nmt = (NMTREEVIEW*)nmhdr;
            FSNode* fsnode = (FSNode*)nmt->itemNew.lParam;
            if ( nmt->action == TVE_COLLAPSE )
            {
                TreeCollapsed(nmt->itemNew.hItem, fsnode);
            }
            return FALSE;
        }
        case TVN_SELCHANGED:
        {
            NMTREEVIEW* nmt = (NMTREEVIEW*)nmhdr;
            FSNode* fsnode = (FSNode*)nmt->itemNew.lParam;
            if ( nmt->action == TVC_BYMOUSE )
            {
                NavigateFromTree(fsnode);
            }
            break;
        }
    }

    return 0;
}