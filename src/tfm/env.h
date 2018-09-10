#pragma once

#include "common.h"

extern const wchar_t ProgramVersion[];

std::wstring const& GetDesktopPath();
std::wstring const& GetMyDocumentsPath();

void InitEnv();