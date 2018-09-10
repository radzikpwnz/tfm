#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "common.h"

#include "mainwnd.h"


const wchar_t ProgramVersion[] = L"v 1.0 Beta";

//static HACCEL hAccelTable;

static bool
Init(HINSTANCE hInstance)
{
    Globals.setHInstance(hInstance);
    Globals.setMainThreadId(GetCurrentThreadId());

    InitCommonControlsEx(NULL);

    InitEnv();

    MainWnd::initCommon();
    FSNode::initCommon();

    //hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TFM));

    return true;
}

int
APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(nCmdShow);

    Debug::init();
    Debug::setLevel(Debug::VERBOSE);

    if ( !Init(hInstance) )
    {
        return 1;
    }

    InstanceManager::create(lpCmdLine);

    MSG msg;
    while ( GetMessage(&msg, NULL, 0, 0) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}


