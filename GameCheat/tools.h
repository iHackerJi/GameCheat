#pragma once
namespace tools
{
    void cheatlog(const char* format, ...);
    unsigned long  findProcessbyName(const wchar_t* name);
    void* findModuleByName(const wchar_t* moduleName, unsigned long pid);;
}
