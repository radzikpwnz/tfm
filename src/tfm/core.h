#pragma once

#include <windows.h>

#include <mutex>

#include "common.h"

#include "mainwnd.h"

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
    HINSTANCE mHInstance;
    DWORD mMainThreadId;

    static bool mCreated;
};

extern Globals_t Globals;


class Instance
{
public:

    ~Instance()
    {
        delete mMainWnd;
    }

    static void entryPoint(Instance* instance);

    MainWnd* getMainWnd() { return mMainWnd; }

private:
    std::wstring mArgs;
    MainWnd* mMainWnd;
    DWORD mWorkerThreadId;

    friend class InstanceManager;
};

class InstanceManager
{
public:

    static Instance* create(std::wstring args);
    static void destroy(Instance* instance);

    static Instance* getCurrent() { return mCurInstance; }

private:
    static void lock() { mutex.lock(); }
    static void unlock() { mutex.unlock(); }

private:
    static unsigned mInstanceCount;
    static std::mutex mutex;
    static thread_local Instance* mCurInstance;

    friend class Instance;
};