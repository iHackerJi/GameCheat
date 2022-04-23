#include "public.h"
namespace cheat
{
    bool aimBot = true;
    bool showBox = true;
	HWND hGame = nullptr;
	HWND hwndCurrent = nullptr;
	unsigned long uGamePid = 0;
	HANDLE hGameProcess = nullptr;

	char * pServer_css_Module = nullptr;
	char* pEngine_Module = nullptr;
}

void cheat::getGameRect(RECT& RectGame)
{
	RECT stRect, stKhRect;
	GetWindowRect(cheat::hGame, &stRect);
	GetClientRect(cheat::hGame, &stKhRect);
	RectGame.left = stRect.left;
	RectGame.right = stRect.right;
	RectGame.top = stRect.bottom - stKhRect.bottom;
	RectGame.bottom = stRect.bottom;
}

bool cheat::readGameMemory(const char * targetAddress,void * buffer,unsigned long size)
{
	SIZE_T readofNumber;
	return	::ReadProcessMemory(cheat::hGameProcess, targetAddress, buffer, size, &readofNumber);
}

void cheat::drawBox(float x,float y,float w,float h,ImColor color,float t)
{
	float cx = x + w;
	float cy = y + h;

	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(cx, y), color, t);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(cx, y), ImVec2(cx, cy), color, t);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(cx, cy), ImVec2(x, cy), color, t);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, cy), ImVec2(x, y), color, t);
}

bool cheat::init(RECT & RectGame)
{
	cheat::hGame = ::FindWindow(CHEAT_GAME_CLASSW, CHEAT_GAME_NAMEW);
	getGameRect(RectGame);
	bool result = false;
	do 
	{
		cheat::uGamePid = tools::findProcessbyName(L"hl2.exe");
		if (cheat::uGamePid == 0)
			break;

		cheat::hGameProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, cheat::uGamePid);
		if (cheat::hGameProcess == nullptr)
			break;

		cheat::pServer_css_Module = (char *)tools::findModuleByName(L"server_css.dll", cheat::uGamePid);
		if (cheat::pServer_css_Module == nullptr)
			break;

		cheat::pEngine_Module = (char*)tools::findModuleByName(L"engine.dll", cheat::uGamePid);
		if (cheat::pEngine_Module == nullptr)
			break;

		result = true;
	} while (false);

	return result;
}

void cheat::unload()
{
	::CloseHandle(cheat::hGame);
}

void cheat::cheatDraw()
{
	RECT RectGame = { 0 };
	RECT RectGameClient = { 0 };
	_role role = { 0 };
	_role player = { 0 };
	float Matrix[4][4] = {0};
	int ViewWidth = 0;
	int ViewHeight = 0;

	int roleNumber = 0; // 角色数量

	cheat::getGameRect(RectGame);
	::SetWindowPos(cheat::hwndCurrent, HWND_TOPMOST, RectGame.left, RectGame.top, RectGame.right - RectGame.left, RectGame.bottom - RectGame.top, SWP_SHOWWINDOW);
	::SetWindowPos(cheat::hwndCurrent, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	ViewWidth = (RectGame.right - RectGame.left) / 2;
	ViewHeight =(RectGame.bottom - RectGame.top) / 2;


	cheat::readGameMemory(pServer_css_Module + 0x3D24B8, &roleNumber,sizeof(roleNumber));
	cheat::readGameMemory(pEngine_Module + 0x48BCB4 - 8, &Matrix, sizeof(Matrix));

	unsigned long playerbaseAddr = 0;
	cheat::readGameMemory(pServer_css_Module + 0x3D24D4 + 0 * 0x10, &playerbaseAddr, sizeof(playerbaseAddr));
	cheat::readGameMemory((char *)playerbaseAddr, &player, sizeof(player)); // 拿玩家数据

	for (int i = 1 ; i < roleNumber ; i++)
	{
		unsigned long rolebaseAddr = 0;
		cheat::readGameMemory(pServer_css_Module + 0x3D24D4 + i * 0x10, &rolebaseAddr, sizeof(rolebaseAddr));
		cheat::readGameMemory((char*)rolebaseAddr, &role, sizeof(role));
		if (role.flag == player.flag || role.hp == 0 || role.hp == 1) // 不绘制队友、死人
			continue;

		float Sc_z;				//人物屏幕Z
		float Sc_x;				//人物屏幕X
		float Sc_foot;		//人物脚部屏幕Y
		float Sc_head;			//人物头部屏幕Y


		Sc_z = Matrix[2][0] * role.x + Matrix[2][1] * role.y + Matrix[2][2] * role.z + Matrix[2][3];
		if (Sc_z < 0) continue;

		float Scale = 1 / Sc_z;		//放缩比例
		Sc_x = ViewWidth + (Matrix[0][0] * role.x+ Matrix[0][1] * role.y + Matrix[0][2] * role.z + Matrix[0][3]) * Scale * ViewWidth;
		Sc_foot = ViewHeight - (Matrix[1][0] * role.x + Matrix[1][1] * role.y + Matrix[1][2] * role .z + Matrix[1][3]) * Scale * ViewHeight;
		Sc_head = ViewHeight - (Matrix[1][0] * role.x + Matrix[1][1] * role.y + Matrix[1][2] * (role.z + 68) + Matrix[1][3]) * Scale * ViewHeight;

		float BoxHeight = (Sc_foot - Sc_head) + 20.0f;
		float BoxWidth = BoxHeight * 0.4f;



		cheat::drawBox(Sc_x - (BoxWidth / 2.0f), Sc_head, Sc_x + (BoxWidth / 2.0f), Sc_head + BoxHeight, ImColor(84, 255, 159, 255), 1.0f);





	}


}