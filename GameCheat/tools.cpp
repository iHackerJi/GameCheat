#include "public.h"

namespace tools
{
    void cheatlog(const char* format, ...);
}

void tools::cheatlog(const char* format, ...)
{
    va_list ArgList;
    PCHAR String = NULL;
    ULONG Length = 0;

    va_start(  ArgList, format);
    Length = _vscprintf(format, ArgList) + 1;
    String = (PCHAR)cheatAllocHeap(Length);
    vsprintf_s(String, Length, format, ArgList);
    OutputDebugStringA(String);

    cheatfreeHeap(String);
    va_end(ArgList);
}