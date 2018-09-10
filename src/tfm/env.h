#pragma once

#include <windows.h>

#include <string>

std::wstring const& GetDesktopPath();
std::wstring const& GetMyDocumentsPath();

void InitEnv();