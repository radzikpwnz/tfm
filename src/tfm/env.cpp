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

#include "env.h"


// Program version string
const wchar_t ProgramVersion[] = L"v 1.0.1";

// Windows path
static std::wstring gWindowsPath;
// Desktop path
static std::wstring gDesktopPath;
// My documents path
static std::wstring gMyDocumentsPath;


// Get windows path
std::wstring const&
GetWindowsPath()
{
    return gWindowsPath;
}

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

    SHGetFolderPath(NULL, CSIDL_WINDOWS, NULL, SHGFP_TYPE_CURRENT, path);
    gWindowsPath = path;

    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, path);
    gDesktopPath = path;

    SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path);
    gMyDocumentsPath = path;
}