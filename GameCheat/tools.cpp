#include "public.h"

namespace tools
{
    void cheatlog(const char *format, ...);
    unsigned long findProcessbyName(const wchar_t *name);
    void* findModuleByName(const wchar_t* moduleName, unsigned long pid);
}

void tools::cheatlog(const char *format, ...)
{
    va_list ArgList;
    PCHAR String = NULL;
    ULONG Length = 0;

    va_start(ArgList, format);
    Length = _vscprintf(format, ArgList) + 1;
    String = (PCHAR)cheatAllocHeap(Length);
    vsprintf_s(String, Length, format, ArgList);
    OutputDebugStringA(String);

    cheatfreeHeap(String);
    va_end(ArgList);
}

unsigned long tools::findProcessbyName(const wchar_t *name)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            if (!wcscmp(pe32.szExeFile, name))
            {
                return pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));

        CloseHandle(hSnapshot);
    }
    return 0;
}

void *tools::findModuleByName(const wchar_t *moduleName, unsigned long pid)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, pid);
    void *ModuleBase = nullptr;
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    MODULEENTRY32 mi;
    mi.dwSize = sizeof(MODULEENTRY32); 
    BOOL bRet = Module32First(hSnapshot, &mi);
    while (bRet)
    {
        if (!wcscmp(mi.szModule, moduleName))
        {
            ModuleBase = mi.modBaseAddr;
        }

        bRet = Module32Next(hSnapshot, &mi);
    }
    CloseHandle(hSnapshot);
    return ModuleBase;
}

