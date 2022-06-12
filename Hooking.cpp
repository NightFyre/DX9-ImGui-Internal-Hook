#include "Hooking.hpp"
namespace DX9Base {
	Hooking::Hooking()
	{
		MH_Initialize();
#if DEVELOPER
		g_Console->printdbg("Hooking::Initialized\n", g_Console->color.pink);
#endif
		return;
	}

	Hooking::~Hooking()
	{
		MH_RemoveHook(MH_ALL_HOOKS);
	}

	/// <summary>
	/// Initializing hook. Everything begins here
	/// - First we obtain information on our target process (Process ID, Window Name, Window Size & Location
	/// - Then we create a transparent window on top of the target process window
	/// </summary>
	void Hooking::Hook()
	{
		g_GameVariables->Initialize();
		g_D3D9Window->Start();
		MH_EnableHook(MH_ALL_HOOKS);
#if DEVELOPER
		g_Console->printdbg("Hooking::Hook Initialized\n", g_Console->color.pink);
#endif
		return;
	}

	/// <summary>
	/// 
	void Hooking::Unhook()
	{
		g_D3D9Window->Stop();
		MH_RemoveHook(MH_ALL_HOOKS);

#if DEVELOPER
		g_Console->DestroyConsole();
#endif

		return;
	}
}
