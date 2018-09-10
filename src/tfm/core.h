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