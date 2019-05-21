#include "main.h"

#include "../../settings.h"
#include "../../ImGUI/imgui_internal.h"
#include "../../Utils/xorstring.h"

#include "../Tabs/aimbottab.h"
#include "../Tabs/triggerbottab.h"
#include "../Tabs/visualstab.h"
#include "../Tabs/hvhtab.h"
#include "../Tabs/misctab.h"
#include "../Tabs/modelstab.h"
#include "../Tabs/skinstab.h"
#include "../Tabs/playerlisttab.h"

#include "../../glhook.h"

bool Main::showWindow = true;

static int ModelAndSkinChangerPage = 0;
void ModelAndSkinChangerTabButtons()
{
	const char* tabs[] = {
		"Models",
		"Skins",
	};
	
	for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
	{
		if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0)))
			ModelAndSkinChangerPage = i;
		
		if (i < IM_ARRAYSIZE(tabs) - 1)
			ImGui::SameLine();
	}
}

void Main::RenderWindow()
{
	if( Settings::UI::Windows::Main::reload )
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Main::posX, Settings::UI::Windows::Main::posY), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Main::sizeX, Settings::UI::Windows::Main::sizeY), ImGuiSetCond_Always);
		Main::showWindow = Settings::UI::Windows::Main::open;
		Settings::UI::Windows::Main::reload = false;
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(Settings::UI::Windows::Main::posX, Settings::UI::Windows::Main::posY), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Main::sizeX, Settings::UI::Windows::Main::sizeY), ImGuiSetCond_FirstUseEver);
	}
	if (!Main::showWindow)
	{
		Settings::UI::Windows::Main::open = false;
		return;
	}
	
	static int page = 0;
	
	if (ImGui::Begin(XORSTR("DeepHook"), &Main::showWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
	{
		Settings::UI::Windows::Main::open = true;
		ImVec2 temp = ImGui::GetWindowSize();
		Settings::UI::Windows::Main::sizeX = (int)temp.x;
		Settings::UI::Windows::Main::sizeY = (int)temp.y;
		temp = ImGui::GetWindowPos();
		Settings::UI::Windows::Main::posX = (int)temp.x;
		Settings::UI::Windows::Main::posY = (int)temp.y;
                const char* tabs[] = {
                        "A", //Aimbot
			"H", //Triggerbot
			"D", //Visuals
			"C", //HvH
			"G", //Visuals
			"I", //Models & Skins Changer
			"E", //Player list
                };
		
		ImGui::PushFont(ImGui::menuFont);
		float size = ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9;
		for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
		{
			if (ImGui::Button(tabs[i], ImVec2(size, size/2.5f)))
				page = i;
			
			if (i < IM_ARRAYSIZE(tabs) - 1)
				ImGui::SameLine();
		}
		ImGui::PopFont();
		
		switch (page)
		{
		case 0:
			Aimbot::RenderTab();
			break;
		case 1:
			Triggerbot::RenderTab();
			break;
		case 2:
			Visuals::RenderTab();
			break;
		case 3:
			HvH::RenderTab();
			break;
		case 4:
			Misc::RenderTab();
			break;
		case 5:
			ModelAndSkinChangerTabButtons();
			ImGui::Separator();
			switch (ModelAndSkinChangerPage)
			{
			case 0:
				Models::RenderTab();
				break;
			case 1:
				Skins::RenderTab();
				break;
			}
			break;
		case 6:
			PlayerList::RenderTab();
			break;
		}
		ImGui::End();
	}
}










