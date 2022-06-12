#pragma once
#include "helper.h"
#include "Console.hpp"

namespace DX9Base {

	class GameData
	{
	public:
		//	CHEATS
		bool bINFINITEAMMO = FALSE;
		bool bINFINITEHEALTH = FALSE;

		explicit GameData();
		~GameData() noexcept = default;
		GameData(GameData const&) = delete;
		GameData(GameData&&) = delete;
		GameData& operator=(GameData const&) = delete;
		GameData& operator=(GameData&&) = delete;

		uintptr_t ResolvePTRS(uintptr_t Ptr, std::vector<unsigned int> Offsets);
	};

	class GameVariables
	{
	public:
		//	Dx & ImGui
		int g_GamePid{};
		HANDLE g_GameHandle{};
		HWND g_GameWindow{};
		int g_GameWidth;
		int g_GameHeight;
		ImVec2 g_WindowSize{};
		LPCSTR g_GameTitle;
		LPCSTR g_ClassName;
		LPCSTR g_GamePath;

		//	MENU RELATED
		bool m_ShowMenu = FALSE;
		bool m_ShowConsole = FALSE;
		bool m_ShowHud{};
		bool m_ShowDemo{};

		explicit GameVariables();
		~GameVariables() noexcept = default;
		GameVariables(GameVariables const&) = delete;
		GameVariables(GameVariables&&) = delete;
		GameVariables& operator=(GameVariables const&) = delete;
		GameVariables& operator=(GameVariables) = delete;
	
		void Initialize();
	};

	inline std::unique_ptr<GameVariables> g_GameVariables;
	inline std::unique_ptr<GameData> g_GameData;
}