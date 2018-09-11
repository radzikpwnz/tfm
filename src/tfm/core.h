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

#include <mutex>

#include "mainwnd.h"

// Global variables (singleton)
class Globals_t
{
public:
    Globals_t()
    {
        if ( mCreated )
        {
            abort();
        }
        mCreated = true;
    }

    HINSTANCE getHInstance() { return mHInstance; }
    void setHInstance(HINSTANCE hInstance) { mHInstance = hInstance; }

    DWORD getMainThreadId() { return mMainThreadId; }
    void setMainThreadId(DWORD id) { mMainThreadId = id; }

private:
    HINSTANCE mHInstance;   // instance handle (win32)
    DWORD mMainThreadId;    // main thread id

    static bool mCreated;   // globals instance created
};

extern Globals_t Globals;


// TFM instance (one independent window)
class Instance
{
public:
    ~Instance()
    {
        delete mMainWnd;
    }

public:
    static void entryPoint(Instance* instance);

public:
    // Get main window
    MainWnd* getMainWnd() { return mMainWnd; }

private:
    std::wstring mArgs;      // arguments (in cmdline format)
    MainWnd* mMainWnd;       // main window
    DWORD mWorkerThreadId;   // instance thread id

    friend class InstanceManager;
};

// TFM instance manager
class InstanceManager
{
public:
    // Get current instance
    static Instance* getCurrent() { return mCurInstance; }

    static Instance* create(std::wstring args);
    static void destroy(Instance* instance);

private:
    static void lock() { mutex.lock(); }
    static void unlock() { mutex.unlock(); }

private:
    static unsigned mInstanceCount;              // instances count
    static std::mutex mutex;                     // instance manager mutex
    static thread_local Instance* mCurInstance;  // current instance

    friend class Instance;
};