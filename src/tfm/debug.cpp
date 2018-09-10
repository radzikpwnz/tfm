#include <fcntl.h>
#include <io.h>

#include "common.h"

Debug::Level Debug::mLevel;

void
Debug::init()
{
    FILE *fp;
    _wfreopen_s(&fp, L"debug_log.txt", L"w", stdout);

    setvbuf(stdout, NULL, _IONBF, 0);
    _setmode(_fileno(stdout), _O_U16TEXT);
}