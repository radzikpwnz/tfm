#pragma once

#include <windows.h>
#include <CommCtrl.h>

#include <cassert>
#include <string>
#include <iostream>
#include <atomic>
#include <filesystem>
namespace fs = std::filesystem;

#include "resource.h"

#include "core.h"
#include "env.h"
#include "debug.h"
#include "fsnode.h"
#include "state.h"
#include "stuff.h"


extern const wchar_t ProgramVersion[];

void RedirectIOToConsole();