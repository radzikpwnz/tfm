#include <windows.h>

#include "common.h"

#include "mainwnd.h"

#include "toolbar.h"

static constexpr unsigned TOOLBAR_ICONS_COUNT = 16;

#define MAINTLB_FIRST_MESSAGE_CODE (WM_APP + 100)


Toolbar::ToolbarBtn Toolbar::buttons[] = {
    //{ L"Back",          0,  &Toolbar::dummyProc   },
    //{ L"Forward",       1,  &Toolbar::dummyProc   },
    { L"Up",            2,  &Toolbar::actUp,        true  },
    { L"Copy",          3,  &Toolbar::actCopy       },
    { L"Cut",           4,  &Toolbar::actCut        },
    { L"Paste",         5,  &Toolbar::actPaste      },
    { L"Delete",        6,  &Toolbar::actDelete,    true  },
    { L"New Folder",    7,  &Toolbar::actNewFolder  }
};


static const wchar_t InstanceProp[] = L"INSTANCE";


LRESULT CALLBACK
Toolbar::wndProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /*switch ( message )
    {
        case WM_SETFOCUS:
        {
            SetFocus((HWND)wParam); 
            return 0;
        }
    }*/

    return mOrigWndProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK
Toolbar::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Toolbar* toolbar = (Toolbar*)GetProp(hWnd, InstanceProp);
    return toolbar->wndProcInternal(hWnd, message, wParam, lParam);
}

bool
Toolbar::addButtons()
{
    const unsigned buttonsCount = sizeof(buttons) / sizeof(buttons[0]);
    const int bitmapSize = 16;

	SendMessage(mHWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    COLORMAP colorMap;
    colorMap.from = RGB(255, 0, 255);
    colorMap.to = GetSysColor(COLOR_BTNFACE);
    HBITMAP hBmp = CreateMappedBitmap(GetModuleHandle(NULL), IDB_TOOLBAR, 0, &colorMap, 1);

	SendMessage(mHWnd, TB_SETINDENT, 2, 0);
	//SendMessage(mHWnd, TB_SETMAXTEXTROWS, 0, 0);

    TBADDBITMAP tb_ab;
	tb_ab.hInst = NULL;
	tb_ab.nID = (UINT_PTR)hBmp;
	SendMessage(mHWnd, TB_ADDBITMAP, buttonsCount, (LPARAM)&tb_ab);

    TBBUTTON tbb_sep = {};
	tbb_sep.fsStyle = BTNS_SEP;

    TBBUTTON tbb;
	tbb.fsState = TBSTATE_ENABLED;
	tbb.fsStyle = BTNS_BUTTON/* | BTNS_AUTOSIZE*/;

    int i, pos;
	for ( i = 0, pos = 0; i < buttonsCount; i++, pos++ )
    {
		tbb.iBitmap = buttons[i].img_id;
		tbb.idCommand = MAINTLB_FIRST_MESSAGE_CODE + i;
		tbb.iString = (INT_PTR)buttons[i].text;

		SendMessage(mHWnd, TB_INSERTBUTTON, pos, (LPARAM)&tbb);

        if ( buttons[i].insert_sep )
        {
            SendMessage(mHWnd, TB_INSERTBUTTON, ++pos, (LPARAM)&tbb_sep);
        }
	}

	return true;
}

void
Toolbar::command(unsigned command)
{
    (this->*buttons[command - MAINTLB_FIRST_MESSAGE_CODE].proc)();
}

Toolbar*
Toolbar::create(HINSTANCE hInstance, MainWnd* parentWnd)
{
    Toolbar* toolbar = new Toolbar();

    toolbar->mHWnd = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
                                    WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE |
                                    /*TBSTYLE_WRAPABLE | *//*TBSTYLE_LIST | */TBSTYLE_FLAT /*| CCS_NODIVIDER*/ | CCS_NORESIZE /*| WS_BORDER*//* | TBSTYLE_TOOLTIPS*/,
                                    0, 0, 0, 0,
                                    parentWnd->hwnd(), NULL, hInstance, NULL);

    if ( toolbar->mHWnd == NULL )
    {
        return nullptr;
    }

    toolbar->addButtons();

    /*DWORD exStyle = SendMessage(toolbar->mHWnd, TB_GETEXTENDEDSTYLE, 0, 0);
    exStyle |= TBSTYLE_EX_DOUBLEBUFFER;
    SendMessage(toolbar->mHWnd, TB_SETEXTENDEDSTYLE, 0, exStyle);*/

    toolbar->mOrigWndProc = (WNDPROC)SetWindowLongPtr(toolbar->mHWnd, GWLP_WNDPROC, (LONG_PTR)wndProc);
    SetProp(toolbar->mHWnd, InstanceProp, toolbar);

    toolbar->mParentWnd = parentWnd;
    UpdateWindow(toolbar->mHWnd);
    return toolbar;
}

void
Toolbar::actDummy()
{

}

void
Toolbar::actUp()
{
    NavigateUp();
}

void
Toolbar::actCopy()
{
    mParentWnd->getContentView()->updateSelectedItems();
    mParentWnd->getContentView()->actCopy();
}

void
Toolbar::actCut()
{
    mParentWnd->getContentView()->updateSelectedItems();
    mParentWnd->getContentView()->actCut();
}

void
Toolbar::actPaste()
{
    mParentWnd->getContentView()->actPaste();
}

void
Toolbar::actDelete()
{
    mParentWnd->getContentView()->updateSelectedItems();
    mParentWnd->getContentView()->actDelete();
}

void
Toolbar::actNewFolder()
{
    mParentWnd->getContentView()->actNewFolder();
}
