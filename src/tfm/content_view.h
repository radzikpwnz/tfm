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
    // Context menu element
    struct CtxMenuElem
    {
        std::wstring text;           // text
        void(ContentView::* proc)(); // handler
    };

    // Selected item
    struct SelItem
    {
        int id;         // id (in list-view context)
        FSNode* fsnode; // FSNode
    };

private:
    static CtxMenuElem ctxOpen;
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
    std::vector<SelItem> mSelItems; // selected items list
    HMENU mCtxMenuHandle;           // context menu handle
};