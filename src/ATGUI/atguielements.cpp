#include "atgui.h"

#include "../Hooks/hooks.h"
#include "../Utils/xorstring.h"
#include "../interfaces.h"
#include "../settings.h"

void UI::KeyBindButton(ButtonCode_t* key)
{
        const char* text = inputSystem->ButtonCodeToString(*key);

        if (SetKeyCodeState::shouldListen && SetKeyCodeState::keyOutput == key)
                text = XORSTR("-- press a key --");
        else
                text = Util::ToUpper(std::string(text)).c_str();

        if (ImGui::Button(text, ImVec2(-1, 0)))
        {
                SetKeyCodeState::shouldListen = true;
                SetKeyCodeState::keyOutput = key;
        }
}

bool UI::ColorPicker(float* col, bool alphabar)
{
        const int EDGE_SIZE = 200; // = int(ImGui::GetWindowWidth() * 0.75f);
        const ImVec2 SV_PICKER_SIZE = ImVec2(EDGE_SIZE, EDGE_SIZE);
        const float SPACING = ImGui::GetStyle().ItemInnerSpacing.x;
        const float HUE_PICKER_WIDTH = 20.f;
        const float CROSSHAIR_SIZE = 7.0f;

        ImColor color(col[0], col[1], col[2]);
        bool value_changed = false;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImVec2 picker_pos = ImGui::GetCursorScreenPos();

        float hue, saturation, value;
        ImGui::ColorConvertRGBtoHSV(color.Value.x, color.Value.y, color.Value.z, hue, saturation, value);

        ImColor colors[] = {
                ImColor(255, 0, 0),
                ImColor(255, 255, 0),
                ImColor(0, 255, 0),
                ImColor(0, 255, 255),
                ImColor(0, 0, 255),
                ImColor(255, 0, 255),
                ImColor(255, 0, 0)
        };

        for (int i = 0; i < 6; i++)
        {
                draw_list->AddRectFilledMultiColor(
                        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING, picker_pos.y + i * (SV_PICKER_SIZE.y / 6)),
                        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH,
                            picker_pos.y + (i + 1) * (SV_PICKER_SIZE.y / 6)),
                        colors[i],
                        colors[i],
                        colors[i + 1],
                        colors[i + 1]
                        );
        }

        draw_list->AddLine(
                ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING - 2, picker_pos.y + hue * SV_PICKER_SIZE.y),
                ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING + 2 + HUE_PICKER_WIDTH, picker_pos.y + hue * SV_PICKER_SIZE.y),
                ImColor(255, 255, 255)
                );

        if (alphabar)
        {
                float alpha = col[3];

                draw_list->AddRectFilledMultiColor(
                        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + HUE_PICKER_WIDTH, picker_pos.y),
                        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + 2 * HUE_PICKER_WIDTH, picker_pos.y + SV_PICKER_SIZE.y),
                        ImColor(0, 0, 0), ImColor(0, 0, 0), ImColor(255, 255, 255), ImColor(255, 255, 255)
                        );

                draw_list->AddLine(
                        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * (SPACING - 2) + HUE_PICKER_WIDTH, picker_pos.y + alpha * SV_PICKER_SIZE.y),
                        ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * (SPACING + 2) + 2 * HUE_PICKER_WIDTH, picker_pos.y + alpha * SV_PICKER_SIZE.y),
                        ImColor(255.f - alpha, 255.f, 255.f)
                        );
        }

        const ImU32 c_oColorBlack = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f));
        const ImU32 c_oColorBlackTransparent = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 0.f));
        const ImU32 c_oColorWhite = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f));

        ImVec4 cHueValue(1, 1, 1, 1);
        ImGui::ColorConvertHSVtoRGB(hue, 1, 1, cHueValue.x, cHueValue.y, cHueValue.z);
        ImU32 oHueColor = ImGui::ColorConvertFloat4ToU32(cHueValue);

        draw_list->AddRectFilledMultiColor(
                ImVec2(picker_pos.x, picker_pos.y),
                ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y),
                c_oColorWhite,
                oHueColor,
                oHueColor,
                c_oColorWhite
                );

        draw_list->AddRectFilledMultiColor(
                ImVec2(picker_pos.x, picker_pos.y),
                ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y),
                c_oColorBlackTransparent,
                c_oColorBlackTransparent,
                c_oColorBlack,
                c_oColorBlack
                );

        float x = saturation * SV_PICKER_SIZE.x;
        float y = (1 - value) * SV_PICKER_SIZE.y;
        ImVec2 p(picker_pos.x + x, picker_pos.y + y);
        draw_list->AddLine(ImVec2(p.x - CROSSHAIR_SIZE, p.y), ImVec2(p.x - 2, p.y), ImColor(255, 255, 255));
        draw_list->AddLine(ImVec2(p.x + CROSSHAIR_SIZE, p.y), ImVec2(p.x + 2, p.y), ImColor(255, 255, 255));
        draw_list->AddLine(ImVec2(p.x, p.y + CROSSHAIR_SIZE), ImVec2(p.x, p.y + 2), ImColor(255, 255, 255));
        draw_list->AddLine(ImVec2(p.x, p.y - CROSSHAIR_SIZE), ImVec2(p.x, p.y - 2), ImColor(255, 255, 255));

        ImGui::InvisibleButton(XORSTR("saturation_value_selector"), SV_PICKER_SIZE);

        if (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])
        {
                ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

                if (mouse_pos_in_canvas.x <     0)
                        mouse_pos_in_canvas.x = 0;
                else if (mouse_pos_in_canvas.x >= SV_PICKER_SIZE.x - 1)
                        mouse_pos_in_canvas.x = SV_PICKER_SIZE.x - 1;

                if (mouse_pos_in_canvas.y < 0)
                        mouse_pos_in_canvas.y = 0;
                else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1)
                        mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

                value = 1 - (mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1));
                saturation = mouse_pos_in_canvas.x / (SV_PICKER_SIZE.x - 1);
                value_changed = true;
        }

        // hue bar logic
        ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SPACING + SV_PICKER_SIZE.x, picker_pos.y));
        ImGui::InvisibleButton(XORSTR("hue_selector"), ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

        if (ImGui::GetIO().MouseDown[0] && (ImGui::IsItemHovered() || ImGui::IsItemActive()))
        {
                ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

                if (mouse_pos_in_canvas.y <     0)
                        mouse_pos_in_canvas.y = 0;
                else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1)
                        mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

                hue = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1);
                value_changed = true;
        }

        if (alphabar)
        {
                ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SPACING * 2 + HUE_PICKER_WIDTH + SV_PICKER_SIZE.x, picker_pos.y));
                ImGui::InvisibleButton(XORSTR("alpha_selector"), ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

                if (ImGui::GetIO().MouseDown[0] && (ImGui::IsItemHovered() || ImGui::IsItemActive()))
                {
                        ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

                        if (mouse_pos_in_canvas.y <     0)
                                mouse_pos_in_canvas.y = 0;
                        else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1)
                                mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

                        float alpha = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1);
                        col[3] = alpha;
                        value_changed = true;
                }
        }

        color = ImColor::HSV(hue >= 1 ? hue - 10 * 1e-6 : hue, saturation > 0 ? saturation : 10*1e-6, value > 0 ? value : 1e-6);
        col[0] = color.Value.x;
        col[1] = color.Value.y;
        col[2] = color.Value.z;

        bool widget_used;
        ImGui::PushItemWidth((alphabar ? SPACING + HUE_PICKER_WIDTH : 0) + SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH - 2*ImGui::GetStyle().FramePadding.x);
        widget_used = alphabar ? ImGui::ColorEdit4("", col) : ImGui::ColorEdit3("", col);
        ImGui::PopItemWidth();

        float new_hue, new_sat, new_val;
        ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], new_hue, new_sat, new_val);

        if (new_hue <= 0 && hue > 0)
        {
                if (new_val <= 0 && value != new_val)
                {
                        color = ImColor::HSV(hue, saturation, new_val <= 0 ? value * 0.5f : new_val);
                        col[0] = color.Value.x;
                        col[1] = color.Value.y;
                        col[2] = color.Value.z;
                }
                else if (new_sat <= 0)
                {
                        color = ImColor::HSV(hue, new_sat <= 0 ? saturation * 0.5f : new_sat, new_val);
                        col[0] = color.Value.x;
                        col[1] = color.Value.y;
                        col[2] = color.Value.z;
                }
        }

        return value_changed | widget_used;
}

bool UI::ColorPicker3(float col[3])
{
        return UI::ColorPicker(col, false);
}

bool UI::ColorPicker4(float col[4])
{
        return UI::ColorPicker(col, true);
}

void UI::SetupColors()
{
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowPadding = ImVec2(4, 4);
        style.WindowMinSize = ImVec2(32, 32);
        style.WindowRounding = 0.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.ChildRounding = 3.0f;
        style.FramePadding = ImVec2(4, 3);
        style.FrameRounding = 3.0f;
        style.ItemSpacing = ImVec2(8, 4);
        style.ItemInnerSpacing = ImVec2(4, 4);
        style.TouchExtraPadding = ImVec2(0, 2);
        style.IndentSpacing = 21.0f;
        style.ColumnsMinSpacing = 3.0f;
        style.ScrollbarSize = 12.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabMinSize = 5.0f;
        style.GrabRounding = 0.0f;
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.DisplayWindowPadding = ImVec2(22, 22);
        style.DisplaySafeAreaPadding = ImVec2(4, 4);
        style.AntiAliasedLines = true;
        style.AntiAliasedFill = true;
        style.CurveTessellationTol = 1.25f;

	ImVec4 accentColor(0.834, 0.418, 0.029, 1.0);

        style.Colors[ImGuiCol_Text]                  = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
        style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_Border]                = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab]         = accentColor;
        style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_CheckMark]             = accentColor;
        style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        style.Colors[ImGuiCol_Button]                = accentColor;
        style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        style.Colors[ImGuiCol_Header]                = accentColor;
        style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip]            = accentColor;
        style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
        style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
}
