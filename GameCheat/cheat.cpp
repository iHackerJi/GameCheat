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

bool cheat::writeGameMemory(const char* targetAddress, void* buffer, unsigned long size)
{
	SIZE_T writeofNumber;
	return	::WriteProcessMemory(global::hGameProcess, (LPVOID)targetAddress, buffer, size, &writeofNumber);
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
	global::hwndGame = ::FindWindow(CHEAT_GAME_CLASSW, NULL);
	getGameRect(RectGame);
	bool result = false;
	do 
	{
		global::uGamePid = tools::findProcessbyName(L"csgo.exe");
		if (global::uGamePid == 0)
			break;

		global::hGameProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, global::uGamePid);
		if (global::hGameProcess == nullptr)
			break;

		global::pClient_Module = (char *)tools::findModuleByName(L"client.dll", global::uGamePid);
		if (global::pClient_Module == nullptr)
			break;

		global::pEngine_Module = (char*)tools::findModuleByName(L"engine.dll", global::uGamePid);
		if (global::pEngine_Module == nullptr)
			break;

		if (cheat::readGameMemory(global::pEngine_Module + hazedumper::signatures::dwClientState, &global::pClient_State, 4) == FALSE)
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

	ViewWidth = (RectGame.right - RectGame.left)/ 2;
	ViewHeight = (RectGame.bottom - RectGame .top)/ 2;


	cheat::readGameMemory(global::pClient_State + hazedumper::signatures::dwClientState_MaxPlayer, &roleNumber,sizeof(roleNumber));
	cheat::readGameMemory(global::pClient_Module + hazedumper::signatures::dwViewMatrix, &Matrix, sizeof(Matrix));

	unsigned long playerbaseAddr = 0;
	cheat::readGameMemory(global::pClient_Module + hazedumper::signatures::dwLocalPlayer, &playerbaseAddr, sizeof(playerbaseAddr));
	//cheat::readGameMemory((char *)playerbaseAddr, &player, sizeof(player)); // 拿玩家数据
	cheat::readGameMemory((char*)playerbaseAddr + hazedumper::netvars::m_iTeamNum, &player.flag, 4);//阵容标识

	for (int i = 0 ; i <= roleNumber ; i++)
	{
		unsigned long rolebaseAddr = 0;
		cheat::readGameMemory(global::pClient_Module + hazedumper::signatures::dwEntityList + i * 0x10, &rolebaseAddr, sizeof(rolebaseAddr));

		cheat::readGameMemory((char*)rolebaseAddr + hazedumper::netvars::m_iTeamNum , &role.flag,4);//阵容标识
		cheat::readGameMemory((char*)rolebaseAddr + hazedumper::netvars::m_iHealth, &role.hp, 4);//拿血量
		cheat::readGameMemory((char*)rolebaseAddr + hazedumper::netvars::m_vecOrigin, &role.x, sizeof(float)*3);

	

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
		Sc_x = Sc_x - 27.0f;//这个Sc_x拿到的是敌人头部中间的位置，所以需要减去一点让它看起来自然点

		Sc_foot = ViewHeight - (Matrix[1][0] * role.x + Matrix[1][1] * role.y + Matrix[1][2] * role .z + Matrix[1][3]) * Scale * ViewHeight;
		Sc_head = ViewHeight - (Matrix[1][0] * role.x + Matrix[1][1] * role.y + Matrix[1][2] * (role.z + 68) + Matrix[1][3]) * Scale * ViewHeight;

		float BoxHeight = (Sc_foot - Sc_head) + 20.0f;
		float BoxWidth = BoxHeight * 0.4f;

		cheat::drawBox(Sc_x, Sc_head,BoxWidth ,  BoxHeight, ImColor(84, 255, 159, 255), 1.0f);

		float hpPercentage = role.hp / 100.0f;
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(Sc_x - 10.0f, Sc_head), ImVec2(Sc_x - 10.0f, (Sc_head + BoxHeight * hpPercentage)), ImColor(247, 9, 104),5.0f );
	}
}



void xyzToAngle(_role player, _role role,float *Angle)
{
	float x = player.x - role.x;
	float y = player.y - role.y;
	float z = player.z - role.z + 65.0f;

	const float pi = 3.1415f;
	Angle[0] = (float)atan(z / sqrt(x * x + y * y)) * 180.0f / pi;
	//算左右角度
	Angle[1] = (float)atan(y / x) * 180.0f / pi;

	if (x >= 0.0f && y >= 0.0f) Angle[1] = Angle[1] - 180.0f; //相对于自己是第四象限
	else if (x < 0.0f && y >= 0.0f) Angle[1] = Angle[1]; //相对于自己而言为第一象限
	else if (x < 0.0f && y < 0.0f) Angle[1] = Angle[1]; //相对于自己而言为第二象限
	else if (x >= 0.0f && y < 0.0f) Angle[1] = Angle[1] + 180.f; //相对于自己而言是第三象限
}


void cheat::aimbot(float maxAngle)
{
	do
	{
		unsigned long playerbaseAddr = 0;
		bool is_fired = false;
		uint32_t hp = 0;
		cheat::readGameMemory(global::pClient_Module + hazedumper::signatures::dwLocalPlayer, &playerbaseAddr, sizeof(playerbaseAddr));
		cheat::readGameMemory((char*)playerbaseAddr + hazedumper::netvars::m_iShotsFired, &is_fired, sizeof(is_fired));
		cheat::readGameMemory((char*)playerbaseAddr + hazedumper::netvars::m_iHealth, &hp, sizeof(hp));
		if (is_fired == false && hp == 0) false;

		float player_view_angles[2] = { 0 };
		_role player = { 0 };
		_role role = { 0 };
		cheat::readGameMemory((char*)global::pClient_State + hazedumper::signatures::dwClientState_ViewAngles, &player_view_angles, sizeof(player_view_angles));//获取自身角度
		cheat::readGameMemory((char*)playerbaseAddr + hazedumper::netvars::m_vecOrigin, &player.x, sizeof(float) * 3);//获取自身角度
		cheat::readGameMemory((char*)playerbaseAddr + hazedumper::netvars::m_iTeamNum, &player.flag, 4);//阵容标识

		int mini_angle[2] = { 0 };//最小角度
		
		float aim_angle[2] = {0};
		bool first = false;
		for (int i = 0; i < 64; i++)
		{
			uint32_t rolebaseAddr = 0;
			cheat::readGameMemory(global::pClient_Module + hazedumper::signatures::dwEntityList + i * 0x10, &rolebaseAddr, sizeof(rolebaseAddr));
			cheat::readGameMemory((char*)rolebaseAddr + hazedumper::netvars::m_iHealth, &role.hp, 4);//拿血量
			cheat::readGameMemory((char*)rolebaseAddr + hazedumper::netvars::m_iTeamNum, &role.flag, 4);//阵容标识

			int bone_id[] = { 8, 4, 3, 7, 6, 5 };
			float role_angle[2] = { 0 };

			for (int j = 0; j < sizeof(bone_id)/sizeof(int) ; j++)
			{
				float boneMtrix[3][4] = {0};

				uint32_t bonebase = 0;
				cheat::readGameMemory((const char*)rolebaseAddr + hazedumper::netvars::m_dwBoneMatrix , &bonebase, sizeof(bonebase));
				cheat::readGameMemory((const char*)bonebase + bone_id[j] * 0x30, &boneMtrix, sizeof(boneMtrix));
				
				if (boneMtrix[0][3] ==0 || boneMtrix[1][3] == 0 || boneMtrix[2][3] == 0 || role.hp == 0 || player.flag == role.flag)
				{
					continue;
				}
				role.x = boneMtrix[0][3];
				role.y = boneMtrix[1][3];
				role.z = boneMtrix[2][3];
				xyzToAngle(player, role, role_angle);

				if (first == false)
				{
					//自身角度xy-减去自瞄视角xy 存入Minimumangle
					mini_angle[0] = abs((int)(player_view_angles[0] - role_angle[0]));
					mini_angle[1] = abs((int)(player_view_angles[1] - role_angle[1]));
					aim_angle[0] = role_angle[0];
					aim_angle[1] = role_angle[1];
					first = true;
				}
				else if ((mini_angle[0] + mini_angle[1]) > ((abs((int)(player_view_angles[0] - role_angle[0]))) + abs((int)(player_view_angles[1] - role_angle[1]))))
				{
					mini_angle[0] = abs((int)(player_view_angles[0] - role_angle[0]));
					mini_angle[1] = abs((int)(player_view_angles[1] - role_angle[1]));
					aim_angle[0] = role_angle[0];
					aim_angle[1] = role_angle[1];
				}
			}
		}
		writeGameMemory(global::pClient_State + hazedumper::signatures::dwClientState_ViewAngles, aim_angle, sizeof(float) * 2);
		Sleep(1000);


	} while (false);






	
	return;
}