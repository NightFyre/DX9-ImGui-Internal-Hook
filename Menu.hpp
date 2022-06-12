#pragma once
#include "helper.h"
#include "Console.hpp"
#include "Game.hpp"
#include "D3D9Window.hpp"
#include "Hooking.hpp"

namespace DX9Base {

	class Menu
	{
	public:
		Menu()  noexcept = default;
		~Menu() noexcept = default;
		Menu(Menu const&) = delete;
		Menu(Menu&&) = delete;
		Menu& operator=(Menu const&) = delete;
		Menu& operator=(Menu&&) = delete;

		//	FORWARD DECLARE FUNCTIONS
		void Draw();
		void MainMenu();
		void HUD(bool* p_open);
		void Loops();
	};
	inline std::unique_ptr<Menu> g_Menu;
}