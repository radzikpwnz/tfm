#include "common.h"

#include "env.h"


// Program version string
const wchar_t ProgramVersion[] = L"v 1.0.0";

// Desktop path
static std::wstring gDesktopPath;
// My documents path
static std::wstring gMyDocumentsPath;


// Get desktop path
std::wstring const&
GetDesktopPath()
{
    return gDesktopPath;
}

// Get my documents path
std::wstring const&
GetMyDocumentsPath()
{
    return gMyDocumentsPath;
}

// Init environment
void
InitEnv()
{
    wchar_t path[MAX_PATH];

    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, path);
    gDesktopPath = path;

    SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path);
    gMyDocumentsPath = path;
}