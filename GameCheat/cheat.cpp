#include "public.h"


namespace cheat
{
    bool aimBot = true;
    bool showBox = true;
}

void cheat::init(RECT & drawSize)
{
    HWND hGame = ::FindWindow(NULL, CHEAT_GAME_NAMEW);
    do 
    {
        CHECK_BOOL(
            ::GetWindowRect(hGame, &drawSize)
        );

    } while (false);
}

void cheat::cheatDraw()
{


}