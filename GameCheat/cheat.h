#pragma once

#define CHEAT_GAME_NAMEW L"" 
namespace cheat
{
    extern bool aimBot;
    extern bool showBox;
    void init(RECT& drawSize);
    void cheatDraw();
}
