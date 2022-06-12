#include "Game.hpp"
namespace DX9Base {
	GameVariables::GameVariables()
	{
#if DEVELOPER
		g_Console->printdbg("GameVariables::Initialized\n", g_Console->color.pink);
#endif
		return;
	}

	//	Get Process Window Information
	bool SelectedWindow = FALSE;
	void GameVariables::Initialize()
	{
		while (!SelectedWindow) {
			DWORD ForegroundWindowProcessID;
			GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
			if (GetCurrentProcessId() == ForegroundWindowProcessID) {
				g_GamePid = GetCurrentProcessId();
				g_GameHandle = GetCurrentProcess();
				g_GameWindow = GetForegroundWindow();

				RECT tempRECT;
				GetWindowRect(g_GameWindow, &tempRECT);
				g_GameWidth = tempRECT.right - tempRECT.left;
				g_GameHeight = tempRECT.bottom - tempRECT.top;

				char tempTitle[MAX_PATH];
				GetWindowTextA(g_GameWindow, tempTitle, sizeof(tempTitle));
				g_GameTitle = tempTitle;

				char tempClassName[MAX_PATH];
				GetClassNameA(g_GameWindow, tempClassName, sizeof(tempClassName));
				g_ClassName = tempClassName;

				char tempPath[MAX_PATH];
				GetModuleFileNameExA(g_GameHandle, NULL, tempPath, sizeof(tempPath));
				g_GamePath = tempPath;
#if DEVELOPER
				g_Console->printdbg("GameData::Init - Process Window Info Established\n", g_Console->color.pink);
#endif
				SelectedWindow = TRUE;
			}
		}
	}

	GameData::GameData()
	{
		dwGameBase = (uintptr_t)GetModuleHandle(g_moduleName);
#if DEVELOPER
		g_Console->printdbg("GameData::Initialized\n", g_Console->color.pink);
#endif
		return;
	}

	/// <summary>
	/// Resolves Address via Base Address & Offsets
	/// </summary>
	/// <param name="Ptr">BaseAddress</param>
	/// <param name="Offsets">Array of offsets</param>
	/// <returns>Address</returns>
	uintptr_t GameData::ResolvePTRS(uintptr_t Ptr, std::vector<unsigned int> Offsets) 
	{
		/// <USAGE EXAMPLE>
		/// uintptr_t PLAYER_OOBJECT = NULL;
		/// uintptr_t BASE_ADDR = 0x69420
		/// std::vector<unsigned int> ptr_Chain = { 0x69. 0x420, 0x666 };
		/// PLAYER_OBJECT = g_GameData->ResolverPTRS(BASE_ADDR, ptr_Chain);
	
		uintptr_t addr = Ptr;
		for (unsigned int i = 0; i < Offsets.size(); i++)
		{
			addr = *(uintptr_t*)addr;
			if (addr == NULL)
				break;
			
			addr += Offsets[i];
		}
		return addr;
	}
}
