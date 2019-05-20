#include "main.h"

#include "../../settings.h"
#include "../../ImGUI/imgui_internal.h"
#include "../../Utils/xorstring.h"

#include "../Tabs/aimbottab.h"
#include "../Tabs/hvhtab.h"
#include "../Tabs/misctab.h"
#include "../Tabs/triggerbottab.h"
#include "../Tabs/visualstab.h"

#include "../../glhook.h"

bool Main::showWindow = true;

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
				"l",
				"l",
				"V",
				"r",
				"M",
		};

		ImGui::PushFont(ImGui::menuFont);
		float width = ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9;
		for (int i = 0; i < IM_ARRAYSIZE(tabs); i++)
		{
			int distance = i == page ? 0 : i > page ? i - page : page - i;

			if (ImGui::Button(tabs[i], ImVec2(width, width/3.f)))
				page = i;

			if (i < IM_ARRAYSIZE(tabs) - 1)
				ImGui::SameLine();
		}
		ImGui::PopFont();

		ImGui::Separator();

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
		}
		ImGui::End();
	}
}
