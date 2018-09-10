#pragma once

#include "common.h"


std::vector<fs::path>& GetClipboard();
void ClearClipboard();
bool GetClipboardIsCut();
void SetClipboardIsCut(bool val);

bool PasteFiles(fs::path dstPath);

bool DeleteFiles(std::vector<fs::path> const& srcPaths);

bool CreateShortcuts(std::vector<fs::path> const& srcPaths, fs::path dstPath);
