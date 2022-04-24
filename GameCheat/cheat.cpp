#include "public.h"
namespace cheat
{
    bool showBox = true;
}

void cheat::getGameRect(RECT& RectGame)
{
	RECT stRect, stKhRect;
	GetWindowRect(global::hwndGame, &stRect);
	GetClientRect(global::hwndGame, &stKhRect);
	RectGame.left = stRect.left;
	RectGame.right = stRect.right;
	RectGame.top = stRect.bottom - stKhRect.bottom;
	RectGame.bottom = stRect.bottom;
}

bool cheat::readGameMemory(const char * targetAddress,void * buffer,unsigned long size)
{
	SIZE_T readofNumber;
	return	::ReadProcessMemory(global::hGameProcess, targetAddress, buffer, size, &readofNumber);
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
	global::hwndGame = ::FindWindow(CHEAT_GAME_CLASSW, CHEAT_GAME_NAMEW);
	getGameRect(RectGame);
	bool result = false;
	do 
	{
		global::uGamePid = tools::findProcessbyName(L"hl2.exe");
		if (global::uGamePid == 0)
			break;

		global::hGameProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, global::uGamePid);
		if (global::hGameProcess == nullptr)
			break;

		global::pServer_css_Module = (char *)tools::findModuleByName(L"server_css.dll", global::uGamePid);
		if (global::pServer_css_Module == nullptr)
			break;

		global::pEngine_Module = (char*)tools::findModuleByName(L"engine.dll", global::uGamePid);
		if (global::pEngine_Module == nullptr)
			break;

		result = true;
	} while (false);

	return result;
}

void cheat::unload()
{
	::CloseHandle(global::hwndGame);
	::CloseHandle(global::hwndCurrent);
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
	if (cheat::showBox == false)
		return;

	cheat::getGameRect(RectGame);
	::SetWindowPos(global::hwndCurrent, HWND_TOPMOST, RectGame.left, RectGame.top, RectGame.right - RectGame.left, RectGame.bottom - RectGame.top, SWP_SHOWWINDOW);
	::SetWindowPos(global::hwndCurrent, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	::GetClientRect(global::hwndGame,&RectGameClient);
	ViewWidth = RectGameClient.right / 2;
	ViewHeight = RectGameClient.bottom/ 2;


	cheat::readGameMemory(global::pServer_css_Module + 0x3D24B8, &roleNumber,sizeof(roleNumber));
	cheat::readGameMemory(global::pEngine_Module + 0x48BCB4 - 0x18, &Matrix, sizeof(Matrix));//这个矩阵找的有点问题，但是能用

	unsigned long playerbaseAddr = 0;
	cheat::readGameMemory(global::pServer_css_Module + 0x3D24D4 + 0 * 0x10, &playerbaseAddr, sizeof(playerbaseAddr));
	cheat::readGameMemory((char *)playerbaseAddr, &player, sizeof(player)); // 拿玩家数据

	for (int i = 1 ; i < roleNumber ; i++)
	{
		unsigned long rolebaseAddr = 0;
		cheat::readGameMemory(global::pServer_css_Module + 0x3D24D4 + i * 0x10, &rolebaseAddr, sizeof(rolebaseAddr));
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

		cheat::drawBox(Sc_x, Sc_head,BoxWidth ,  BoxHeight, ImColor(84, 255, 159, 255), 1.0f);

		float hpPercentage = role.hp / 100.0f;
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(Sc_x - 10, Sc_head), ImVec2(Sc_x - 10, (Sc_head + BoxHeight * hpPercentage)), ImColor(247, 9, 104),5.0f );
	}
}