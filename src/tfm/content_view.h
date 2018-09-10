#pragma once

#include "common.h"

#include "mainwnd_fwd.h"

class ContentView
{
public:
    enum ViewStyle
    {
        LARGE_ICON,
        SMALL_ICON,
        LIST,
        DETAILS
    };

    enum ContextMenuType
    {
        MENU_SINGLE,
        MENU_MULTIPLE,
        MENU_EMPTY
    };

private:
    struct CtxMenuElem
    {
        std::wstring text;
        void(ContentView::* proc)();
    };

    struct SelItem
    {
        int id;
        FSNode* fsnode;
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
    HWND hwnd() { return mHWnd; }

public:
    static ContentView* create(HINSTANCE hInstance, MainWnd* parentWnd);

    void setViewStyle(ViewStyle style);

    uint32_t getSelectedItems(std::vector<SelItem>& resList);

    void updateSelectedItems()
    {
        getSelectedItems(mSelItems);
    }

    void refreshContent();

    void insertCtxMenuElem(unsigned id, CtxMenuElem* elem, bool insertSep = false);
    void createContextMenu(ContextMenuType type);
    LRESULT notify(NMHDR* nmhdr);

    void actDummy();
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
    LRESULT CALLBACK wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void insertColumns();

private:
    HWND mHWnd;
    WNDPROC mOrigWndProc;
    MainWnd* mParentWnd;
    ViewStyle mViewStyle;
    std::vector<SelItem> mSelItems;
    HMENU mCtxMenuHandle;
};