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