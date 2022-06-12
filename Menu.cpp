#include "Menu.hpp"
namespace DX9Base {
	void Menu::Draw()
	{
		if (g_GameVariables->m_ShowMenu)
			MainMenu();

		if (g_GameVariables->m_ShowHud)
			HUD(&g_GameVariables->m_ShowHud);

		if (g_GameVariables->m_ShowDemo)
			ImGui::ShowDemoWindow();
	}

	void Menu::MainMenu()
	{
		if (!ImGui::Begin("[DX9] ImGui Internal Base", &g_GameVariables->m_ShowMenu, 96))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("MAIN"))
			{
				//Tabs::TABMain();
				ImGui::Text("[DX9] ImGui INTERNAL MENU (PREVIEW)");
				ImGui::Text("BUILD VERSION: v1.0");
				ImGui::Text("BUILD DATE: 5/30/2022");
				ImGui::Checkbox("SHOW IMGUI DEMO", &g_GameVariables->m_ShowDemo);
				if (ImGui::Checkbox("SHOW DEBUG CONSOLE", &g_GameVariables->m_ShowConsole)) {
					switch (g_GameVariables->m_ShowConsole) {
					case(TRUE):
						ShowWindow(g_Console->g_hWnd, SW_SHOW);
						break;
					case(FALSE):
						ShowWindow(g_Console->g_hWnd, SW_HIDE);
						break;
					}
				}
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::Button("UNHOOK DLL", ImVec2(ImGui::GetWindowContentRegionWidth() - 3, 20))) g_Killswitch = TRUE;
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}
	
	void Menu::HUD(bool* p_open)
	{
		//	Here you can draw a secondary window to show the user what menu items they have active
		// This makes it easier on the user , include a method to show and hide the HUD

	}
	
	void Menu::Loops()
	{
		///	Code inside this function body will execute every 1ms
		// Best used for constant writes and for anything that you need written while the menu is not shown

		if (g_GameData->bINFINITEHEALTH) {

		}

		if (g_GameData->bINFINITEAMMO) {
			
		}
	}
}
