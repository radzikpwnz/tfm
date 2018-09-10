#include "common.h"

#include <shlobj.h>

#include "env.h"

static std::wstring gDesktopPath;
static std::wstring gMyDocumentsPath;


std::wstring const&
GetDesktopPath()
{
    return gDesktopPath;
}

std::wstring const&
GetMyDocumentsPath()
{
    return gMyDocumentsPath;
}

void
InitEnv()
{
    wchar_t path[MAX_PATH];

    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, path);
    gDesktopPath = path;

    SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path);
    gMyDocumentsPath = path;
}