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

#include "fsnode.h"
#include "mainwnd_fwd.h"

// Content window (List-View Control)
class ContentView
{
public:
    // View style
    enum ViewStyle
    {
        LARGE_ICON,
        SMALL_ICON,
        LIST,
        DETAILS
    };

    // Context menu type
    enum ContextMenuType
    {
        MENU_SINGLE,   // when single element selected
        MENU_MULTIPLE, // when multiple elements selected
        MENU_EMPTY     // when no elements selected (click on empty space)
    };

private:
    // Item
    struct Item
    {
        FSNode const* fsnode;
        int image;
        std::wstring text;
        std::wstring size;
        std::wstring type;
    };

    // Context menu element
    struct CtxMenuElem
    {
        std::wstring text;           // text
        void(ContentView::* proc)(); // handler
    };

    // Selected item
    struct SelItem
    {
        int id;               // id (in list-view context)
        FSNode const* fsnode; // FSNode
    };

private:
    static CtxMenuElem ctxOpen;
    static CtxMenuElem ctxEdit;
    static CtxMenuElem ctxCut;
    static CtxMenuElem ctxCopy;
    static CtxMenuElem ctxCreateLink;
    static CtxMenuElem ctxDelete;
    static CtxMenuElem ctxRename;
    static CtxMenuElem ctxPaste;
    static CtxMenuElem ctxPasteAsLink;
    static CtxMenuElem ctxNewFolder;

public:
    static ContentView* create(HINSTANCE hInstance, MainWnd* parentWnd);

private:
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
    // Get hwnd
    HWND hwnd() { return mHWnd; }

    // Update selected items list
    void updateSelectedItems()
    {
        getSelectedItems(mSelItems);
    }

    void setViewStyle(ViewStyle style);

    void refreshContent();

    LRESULT notify(NMHDR* nmhdr);

    void actOpen();
    void actEdit();
    void actCut();
    void actCopy();
    void actCreateLink();
    void actDelete();
    void actRename();
    void actPaste();
    void actPasteAsLink();
    void actNewFolder();

private:
    uint32_t getSelectedItems(std::vector<SelItem>& resList);

    void insertCtxMenuElem(unsigned id, CtxMenuElem* elem, bool insertSep = false);
    void createContextMenu(ContextMenuType type);

    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void insertColumns();

private:
    HWND mHWnd;                     // hwnd
    MainWnd* mParentWnd;            // parent window
    WNDPROC mOrigWndProc;           // orig window procedure
    ViewStyle mViewStyle;           // view style
    std::vector<Item> mItems;       // items
    std::vector<SelItem> mSelItems; // selected items list
    HMENU mCtxMenuHandle;           // context menu handle
};