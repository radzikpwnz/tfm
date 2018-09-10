#include <fcntl.h>
#include <io.h>

#include "common.h"

Debug::Level Debug::mLevel = DISABLE;

bool
Debug::init()
{
    FILE *fp;
    _wfreopen_s(&fp, L"debug_log.txt", L"w", stdout);

    if ( fp == nullptr )
    {
        return false;
    }

    setvbuf(stdout, NULL, _IONBF, 0);
    _setmode(_fileno(stdout), _O_U16TEXT);

    return true;
}