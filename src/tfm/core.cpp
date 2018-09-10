#include "common.h"

#include "mainwnd.h"
#include "state.h"

#include "core.h"


// Global variables instance
Globals_t Globals;
bool Globals_t::mCreated = false;

unsigned InstanceManager::mInstanceCount;
std::mutex InstanceManager::mutex;
thread_local Instance* InstanceManager::mCurInstance;


// Parce instance arguments
static void
ParseArgs(std::wstring& args)
{
    if ( args.empty() )
    {
        SetCurPathFromCmdlineArg(L"");
        return;
    }

    int argc;
    wchar_t** argv = CommandLineToArgvW(args.c_str(), &argc);

    SetCurPathFromCmdlineArg(argv[0]);
}

// Instance thread entry point
void
Instance::entryPoint(Instance* instance)
{
    InstanceManager::mCurInstance = instance;

    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    InitRootFSNodes();
    InitFSNodesTree();

    instance->mMainWnd = MainWnd::create(Globals.getHInstance());
    if ( instance->mMainWnd == nullptr )
    {
        Debug::logErr(L"Failed to create main window!\n");
        return;
    }

    ShowWindow(instance->mMainWnd->hwnd(), SW_SHOW);
    UpdateWindow(instance->mMainWnd->hwnd());

    Debug::logErr(L"Args: %s\n", instance->mArgs.c_str());
    ParseArgs(instance->mArgs);

    MSG msg;
    while ( GetMessage(&msg, NULL, 0, 0) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();

    InstanceManager::destroy(instance);
}

// Create new instance
Instance*
InstanceManager::create(std::wstring args)
{
    lock();

    Debug::logVerbose(L"Creating instance... (current count %u)\n", mInstanceCount);

    Instance* instance = new Instance();
    instance->mArgs = args;
    std::thread thr(Instance::entryPoint, instance);
    thr.detach();
    mInstanceCount++;

    unlock();

    return instance;
}

// Destroy instance
void
InstanceManager::destroy(Instance* instance)
{
    lock();

    Debug::logVerbose(L"Destroying instance... (current count %u)\n", mInstanceCount);

    delete instance;
    mInstanceCount--;

    if ( mInstanceCount == 0 )
    {
        Debug::logVerbose(L"No more instances! Posting WM_QUIT...\n");
        PostThreadMessage(Globals.getMainThreadId(), WM_QUIT, 0, 0);
    }

    unlock();
}