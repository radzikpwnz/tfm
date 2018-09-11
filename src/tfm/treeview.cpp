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

#include "fsnode.h"
#include "state.h"
#include "mainwnd.h"

#include "treeview.h"


// Clear and rebuild all item childs
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

// Clear and rebuild all content
void
TreeView::refreshContent()
{
    TreeView_DeleteAllItems(mHWnd);
    mHOpenedItem = NULL;

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

// WM_NOTIFY message handler
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
        case NM_CLICK:
        {
            TVHITTESTINFO hti;
            GetCursorPos(&hti.pt);
            ScreenToClient(mHWnd, &hti.pt);
            TreeView_HitTest(mHWnd, &hti);

            if ( hti.hItem != NULL && (hti.flags & TVHT_ONITEM)/* && mHOpenedItem != hti.hItem*/ )
            {
                TVITEM item;
                item.hItem = hti.hItem;
                item.mask = TVIF_PARAM;
                TreeView_GetItem(mHWnd, &item);
                FSNode* fsnode = (FSNode*)item.lParam;
                mHOpenedItem = hti.hItem;
                NavigateFromTree(fsnode);
            }
            break;
        }
        /*case TVN_SELCHANGED:
        {
            NMTREEVIEW* nmt = (NMTREEVIEW*)nmhdr;
            FSNode* fsnode = (FSNode*)nmt->itemNew.lParam;
            if ( nmt->action == TVC_BYMOUSE && mHOpenedItem != nmt->itemNew.hItem )
            {
                mHOpenedItem = nmt->itemNew.hItem;
                NavigateFromTree(fsnode);
            }
            break;
        }*/
    }

    return 0;
}

// Create tree-view instance
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

    treeView->mParentWnd = parentWnd;
    UpdateWindow(treeView->mHWnd);
    return treeView;
}