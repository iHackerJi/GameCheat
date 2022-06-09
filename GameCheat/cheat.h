#pragma once
#define CHEAT_GAME_CLASSW L"Valve001" 
#define CHEAT_GAME_NAMEW L"Counter-Strike Source" 

#pragma pack (1)
typedef struct _role
{
    char unknow1[0x9c]; // 
    unsigned long hp;   // +9c
    char unknow2[0x18c];
    unsigned long flag;// 3 = ¾¯²ì ¡¢ 2 = º··Ë
    char unknow3[0x8c];// + a0
    float x;
    float y;
    float z;
}_role;
#pragma pack ()

namespace cheat
{
    extern bool showBox;

    bool init(RECT& RectGame);
    void getGameRect(RECT& RectGame);
    void unload();
    void cheatDraw();
    bool readGameMemory(const char* targetAddress, void* buffer, unsigned long size);
    bool writeGameMemory(const char* targetAddress, void* buffer, unsigned long size);
    void drawBox(float x, float y, float cx, float cy, ImColor color, float w);
    void aimbot(float maxAngle);
}
