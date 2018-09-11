#include "common.h"

#include "core.h"
#include "resource.h"
#include "state.h"

#include "actions.h"


// Clipboard (contains full paths to files/folders)
static thread_local std::vector<fs::path> gClipboard;
// Cut mode flag for current clipboard content
static thread_local bool gClipboardIsCut;


// Async file operation action
class FileOpAsyncAction
{
public:
    // Action type
    enum ActionType
    {
        ACT_DELETE,  // delete
        ACT_COPY,    // copy
        ACT_MOVE     // move (rename)
    };

private:
    // Messages (from worker thread to dialog (in main thread))
    enum Msg
    {
        MSG_CLOSE = WM_APP,     // close dialog and finish action
        MSG_NAVIGATE_REFRESH,   // refresh navigation content
        MSG_ERROR               // error during operation
    };

private:
    static void workerEntryPoint(FileOpAsyncAction* act);
    static INT_PTR CALLBACK dlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
    FileOpAsyncAction(ActionType type)
        : mType(type), mSrcPaths(), mDstPath(), mRes(false), mCancel(false)
    {
    }

    // Set source paths
    void setSrcPaths(std::vector<fs::path> const& srcPaths) { mSrcPaths = srcPaths; }
    // Set destination path
    void setDstPath(fs::path const& dstPaths) { mDstPath = dstPaths; }

    void run();

private:
    void workerEntryPointInternal();
    INT_PTR CALLBACK dlgProcInternal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

private:
    ActionType mType;                  // action type
    HWND mHDlg;                        // dialog hwnd
    HWND mCurPbHwnd;                   // current progress bar hwnd
    HWND mTotalPbHWnd;                 // total progress bar hwnd
    std::vector<fs::path> mSrcPaths;   // source paths
    fs::path mDstPath;                 // destination path

    std::atomic<bool> mRes;            // [out] operation result
    std::atomic<bool> mCancel;         // cancel flag (signal from worker thread to dialog)
};

// Dialog procedure (internal function)
INT_PTR CALLBACK
FileOpAsyncAction::dlgProcInternal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch ( message )
    {
        case WM_INITDIALOG:
        {
            RECT rect, rect1;
            GetWindowRect(GetParent(hDlg), &rect);
            GetClientRect(hDlg, &rect1);
            SetWindowPos(hDlg, NULL, (rect.right + rect.left) / 2 - (rect1.right - rect1.left) / 2,
                         (rect.bottom + rect.top) / 2 - (rect1.bottom - rect1.top) / 2,
                         0, 0, SWP_NOZORDER | SWP_NOSIZE);

            mHDlg = hDlg;

            mCurPbHwnd = GetDlgItem(mHDlg, IDC_PROGRESS_CUR);
            mTotalPbHWnd = GetDlgItem(mHDlg, IDC_PROGRESS_TOTAL);

            SetWindowText(mHDlg, L"Preparing...");
            SetWindowText(GetDlgItem(mHDlg, IDC_CURFILE), L"Preparing...");

            SetWindowLongPtr(mCurPbHwnd, GWL_STYLE, GetWindowLongPtr(mCurPbHwnd, GWL_STYLE) | PBS_MARQUEE);
            SendMessage(mCurPbHwnd, PBM_SETMARQUEE, 1, 100);

            SetWindowLongPtr(mTotalPbHWnd, GWL_STYLE, GetWindowLongPtr(mTotalPbHWnd, GWL_STYLE) | PBS_MARQUEE);
            SendMessage(mTotalPbHWnd, PBM_SETMARQUEE, 1, 100);

            return (INT_PTR)TRUE;
        }
        case WM_COMMAND:
        {
            if ( LOWORD(wParam) == IDCANCEL )
            {
                if ( MessageBox(hDlg, L"Cancel operation?", L"Cancel", MB_YESNO | MB_ICONQUESTION) == IDYES )
                {
                    mCancel = true;
                }
            }
            break;
        }
        case MSG_CLOSE:
        {
            DestroyWindow(hDlg);
            NavigateRefresh();
            delete this;
            break;
        }
        case MSG_NAVIGATE_REFRESH:
        {
            NavigateRefresh();
            break;
        }
        case MSG_ERROR:
        {
            MessageBox(hDlg, L"Error!", L"Error", MB_OK | MB_ICONERROR);
            break;
        }
    }

    return FALSE;
}

// Dialog procedure
INT_PTR CALLBACK
FileOpAsyncAction::dlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    if ( message == WM_INITDIALOG )
    {
        SetProp(hDlg, L"INSTANCE", (HANDLE)lParam);
    }

    FileOpAsyncAction* act = (FileOpAsyncAction*)GetProp(hDlg, L"INSTANCE");
    return act->dlgProcInternal(hDlg, message, wParam, lParam);
}

// Worker thread entry point (internal function)
void
FileOpAsyncAction::workerEntryPointInternal()
{
    std::chrono::time_point<std::chrono::steady_clock> lastRefreshTime = std::chrono::steady_clock::now();

    if ( mType == ACT_DELETE )
    {
        SetWindowText(mHDlg, L"Deleting files...");

        for ( unsigned i = 0; i < mSrcPaths.size(); i++ )
        {
            SetWindowText(GetDlgItem(mHDlg, IDC_CURFILE), mSrcPaths[i].c_str());

            /*std::error_code err;
            if ( !fs::remove_all(mSrcPaths[i], err) || err )
            {
                Debug::logErr(L"Error deleting %s, err %d\n", mSrcPaths[i].c_str(), err.value());
                SendMessage(mHDlg, MSG_ERROR, 0, 0);
                break;
            }*/

            // Prepare path for SHFILEOPSTRUCT format (double null-terminate)
            std::wstring curSrcFixed = mSrcPaths[i].wstring();
            curSrcFixed.resize(curSrcFixed.length() + 2);

            SHFILEOPSTRUCT s = { 0 };
            s.hwnd = mHDlg;
            s.wFunc = FO_DELETE;
            s.fFlags = FOF_SILENT;
            s.pTo = NULL;
            s.pFrom = curSrcFixed.c_str();
            int res = SHFileOperation(&s);
            if ( res != 0 )
            {
                Debug::logErr(L"Error deleting %s, err %d\n", mSrcPaths[i].c_str(), GetLastError());
                SendMessage(mHDlg, MSG_ERROR, 0, 0);
                break;
            }

            std::chrono::time_point<std::chrono::steady_clock> curTime = std::chrono::steady_clock::now();
            if ( std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastRefreshTime).count() >= 3000 )
            {
                SendMessage(mHDlg, MSG_NAVIGATE_REFRESH, 0, 0);
                lastRefreshTime = curTime;
            }

            if ( mCancel )
            {
                break;
            }
        }
    } else if ( mType == ACT_COPY || mType == ACT_MOVE )
    {
        SetWindowText(mHDlg, mType == ACT_COPY ? L"Copying files..." : L"Moving files...");
        SetWindowText(GetDlgItem(mHDlg, IDC_CURFILE), L"...");

        for ( unsigned i = 0; i < mSrcPaths.size(); i++ )
        {
            SetWindowText(GetDlgItem(mHDlg, IDC_CURFILE), mSrcPaths[i].c_str());
           
            fs::path const& curSrc = mSrcPaths[i];
            fs::path curDst = mDstPath / curSrc.filename();

            if ( mType == ACT_COPY )
            {
                /*std::error_code err;
                fs::copy(curSrc, curDst, err);
                if ( err )
                {
                    Debug::logErr(L"Error copying %s to %s, err %d\n", curSrc.c_str(), curDst.c_str(), err.value());
                    SendMessage(mHDlg, MSG_ERROR, 0, 0);
                    break;
                }*/

                // Prepare paths for SHFILEOPSTRUCT format (double null-terminate)
                std::wstring curSrcFixed = curSrc.wstring();
                curSrcFixed.resize(curSrcFixed.length() + 2);
                std::wstring dstFixed = mDstPath.wstring();
                dstFixed.resize(dstFixed.length() + 2);

                SHFILEOPSTRUCT s = { 0 };
                s.hwnd = mHDlg;
                s.wFunc = FO_COPY;
                s.fFlags = FOF_SILENT;
                s.pTo = dstFixed.c_str();
                s.pFrom = curSrcFixed.c_str();
                int res = SHFileOperation(&s);
                if ( res != 0 )
                {
                    Debug::logErr(L"Error copying %s to %s, err %d\n", curSrc.c_str(), curDst.c_str(), GetLastError());
                    SendMessage(mHDlg, MSG_ERROR, 0, 0);
                    break;
                }
            } else
            {
                /*std::error_code err;
                fs::rename(curSrc, curDst, err);
                if ( err )
                {
                    Debug::logErr(L"Error moving %s to %s, err %d\n", curSrc.c_str(), curDst.c_str(), err.value());
                    SendMessage(mHDlg, MSG_ERROR, 0, 0);
                    break;
                }*/

                // Prepare paths for SHFILEOPSTRUCT format (double null-terminate)
                std::wstring curSrcFixed = curSrc.wstring();
                curSrcFixed.resize(curSrcFixed.length() + 2);
                std::wstring dstFixed = mDstPath.wstring();
                dstFixed.resize(dstFixed.length() + 2);

                SHFILEOPSTRUCT s = { 0 };
                s.hwnd = mHDlg;
                s.wFunc = FO_MOVE;
                s.fFlags = FOF_SILENT;
                s.pTo = dstFixed.c_str();
                s.pFrom = curSrcFixed.c_str();
                int res = SHFileOperation(&s);
                if ( res != 0 )
                {
                    Debug::logErr(L"Error moving %s to %s, err %d\n", curSrc.c_str(), curDst.c_str(), GetLastError());
                    SendMessage(mHDlg, MSG_ERROR, 0, 0);
                    break;
                }
            }

            auto curTime = std::chrono::steady_clock::now();
            if ( std::chrono::duration_cast<std::chrono::milliseconds>(curTime - lastRefreshTime).count() >= 3000 )
            {
                SendMessage(mHDlg, MSG_NAVIGATE_REFRESH, 0, 0);
                lastRefreshTime = curTime;
            }

            if ( mCancel )
            {
                break;
            }
        }
    }
    
    SendMessage(mHDlg, MSG_CLOSE, 0, 0);
}

// Worker thread entry point
void
FileOpAsyncAction::workerEntryPoint(FileOpAsyncAction* act)
{
    act->workerEntryPointInternal();
}

// Run action
void
FileOpAsyncAction::run()
{
    mHDlg = CreateDialogParam(Globals.getHInstance(), MAKEINTRESOURCE(IDD_FILEOP_PROGRESS), InstanceManager::getCurrent()->getMainWnd()->hwnd(), dlgProc, (LPARAM)this);
    ShowWindow(mHDlg, SW_SHOW);

    std::thread worker(workerEntryPoint, this);
    worker.detach();
}


// Get clipboard contents
std::vector<fs::path>&
GetClipboard()
{
    return gClipboard;
}

// Clear clipboard
void
ClearClipboard()
{
    gClipboard.clear();
}

// Is current clipboard in cut mode
bool
GetClipboardIsCut()
{
    return gClipboardIsCut;
}

// Set current clipboard in cut mode
void
SetClipboardIsCut(bool val)
{
    gClipboardIsCut = val;
}


// Paste file(s)
bool
PasteFiles(fs::path dstPath)
{
    if ( gClipboard.empty() )
    {
        return true;
    }

    FileOpAsyncAction* act = new FileOpAsyncAction(gClipboardIsCut ? FileOpAsyncAction::ACT_MOVE : FileOpAsyncAction::ACT_COPY);

    act->setSrcPaths(gClipboard);
    act->setDstPath(dstPath);

    if ( gClipboardIsCut )
    {
        ClearClipboard();
    }

    act->run();

    return true;
}

// Delete file(s)
bool
DeleteFiles(std::vector<fs::path> const& srcPaths)
{
    if ( srcPaths.empty() )
    {
        return true;
    }

    FileOpAsyncAction* act = new FileOpAsyncAction(FileOpAsyncAction::ACT_DELETE);

    act->setSrcPaths(srcPaths);

    act->run();

    return true;
}

// Create one shortcut
static bool
CreateShortcut(std::wstring const& src, std::wstring const& dst)
{
    HRESULT hres;

    IShellLink* psl;
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);

    if ( SUCCEEDED(hres) )
    {
        psl->SetPath(src.c_str());
        psl->SetDescription(L"");

        IPersistFile* ppf;
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if ( SUCCEEDED(hres) )
        {
            hres = ppf->Save(dst.c_str(), TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return SUCCEEDED(hres);
}

// Create shortcut(s)
bool
CreateShortcuts(std::vector<fs::path> const& srcPaths, fs::path dstPath)
{
    for ( unsigned i = 0; i < srcPaths.size(); i++ )
    {
        fs::path const& curSrc = srcPaths[i];
        fs::path curDst = dstPath / curSrc.filename();
        curDst.replace_extension(L"lnk");
        CreateShortcut(curSrc, curDst);
    }

    return true;
}