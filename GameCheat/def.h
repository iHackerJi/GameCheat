#pragma once

#define cheatAllocHeap(size) memset(malloc(size),0,size)
#define cheatfreeHeap(addr)  free(addr);

#define CHECK_BOOL(Call)\
{\
    if (Call == false)\
    {\
        tools::cheatlog("game_cheat %s->%s error! lastError=%d", __FUNCTION__,#Call, GetLastError());\
        exit(0);\
    }\
}