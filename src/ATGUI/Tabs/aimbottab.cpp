#include "aimbottab.h"

#include "../../SDK/definitions.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../bezier.h"

static ItemDefinitionIndex currentWeapon = ItemDefinitionIndex::INVALID;
static bool enabled = false;
static bool silent = false;
static bool friendly = false;
static bool closestBone = false;
static bool desiredBones[] = {true, true, true, true, true, true, true, // center mass
                              false, false, false, false, false, false, false, // left arm
                              false, false, false, false, false, false, false, // right arm
                              false, false, false, false, false, // left leg
                              false, false, false, false, false  // right leg
};
static bool engageLock = false;
static bool engageLockTR = false; // Target Reacquisition
static int engageLockTTR = 700; // Time to Target Reacquisition ( in ms )
static Bone bone = Bone::BONE_HEAD;
static ButtonCode_t aimkey = ButtonCode_t::MOUSE_MIDDLE;
static bool aimkeyOnly = false;
static bool smoothEnabled = false;
static float smoothValue = 0.5f;
static SmoothType smoothType = SmoothType::SLOW_END;
static bool smoothSaltEnabled = false;
static float smoothSaltMultiplier = 0.0f;
static bool errorMarginEnabled = false;
static float errorMarginValue = 0.0f;
static bool curveEnabled = false;
static float curveValue[5] = { 0.390f, 0.575f, 0.565f, 0.900f };
static bool autoAimEnabled = false;
static float autoAimValue = 180.0f;
static bool aimStepEnabled = false;
static float aimStepMin = 25.0f;
static float aimStepMax = 35.0f;
static bool rcsEnabled = false;
static bool rcsAlwaysOn = false;
static float rcsAmountXMin = 0.5f;
static float rcsAmountXMax = 1.5f;
static float rcsAmountXSpeed = 0.1f;
static float rcsAmountYMin = 0.5f;
static float rcsAmountYMax = 1.5f;
static float rcsAmountYSpeed = 0.5f;
static bool autoPistolEnabled = false;
static bool autoShootEnabled = false;
static bool autoScopeEnabled = false;
static bool noShootEnabled = false;
static bool ignoreJumpEnabled = false;
static bool ignoreEnemyJumpEnabled = false;
static bool smokeCheck = false;
static bool flashCheck = false;
static bool spreadLimitEnabled = false;
static float spreadLimit = 0.1f;
static bool autoWallEnabled = false;
static float autoWallValue = 10.0f;
static bool autoAimRealDistance = false;
static bool autoSlow = false;
static bool predEnabled = false;

void UI::ReloadWeaponSettings()
{
        ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
        if (Settings::Aimbot::weapons.find(currentWeapon) != Settings::Aimbot::weapons.end())
                index = currentWeapon;

        enabled = Settings::Aimbot::weapons.at(index).enabled;
        silent = Settings::Aimbot::weapons.at(index).silent;
        friendly = Settings::Aimbot::weapons.at(index).friendly;
        closestBone = Settings::Aimbot::weapons.at(index).closestBone;
        engageLock = Settings::Aimbot::weapons.at(index).engageLock;
        engageLockTR = Settings::Aimbot::weapons.at(index).engageLockTR;
        engageLockTTR = Settings::Aimbot::weapons.at(index).engageLockTTR;
        bone = Settings::Aimbot::weapons.at(index).bone;
        aimkey = Settings::Aimbot::weapons.at(index).aimkey;
        aimkeyOnly = Settings::Aimbot::weapons.at(index).aimkeyOnly;
        smoothEnabled = Settings::Aimbot::weapons.at(index).smoothEnabled;
        smoothValue = Settings::Aimbot::weapons.at(index).smoothAmount;
        smoothType = Settings::Aimbot::weapons.at(index).smoothType;
        smoothSaltEnabled = Settings::Aimbot::weapons.at(index).smoothSaltEnabled;
        smoothSaltMultiplier = Settings::Aimbot::weapons.at(index).smoothSaltMultiplier;
        errorMarginEnabled = Settings::Aimbot::weapons.at(index).errorMarginEnabled;
        errorMarginValue = Settings::Aimbot::weapons.at(index).errorMarginValue;
	curveEnabled = Settings::Aimbot::weapons.at(index).curveEnabled;
        autoAimEnabled = Settings::Aimbot::weapons.at(index).autoAimEnabled;
        autoAimValue = Settings::Aimbot::weapons.at(index).autoAimFov;
        aimStepEnabled = Settings::Aimbot::weapons.at(index).aimStepEnabled;
        aimStepMin = Settings::Aimbot::weapons.at(index).aimStepMin;
        aimStepMax = Settings::Aimbot::weapons.at(index).aimStepMax;
        rcsEnabled = Settings::Aimbot::weapons.at(index).rcsEnabled;
        rcsAlwaysOn = Settings::Aimbot::weapons.at(index).rcsAlwaysOn;
        rcsAmountXMin = Settings::Aimbot::weapons.at(index).rcsAmountXMin;
        rcsAmountXMax = Settings::Aimbot::weapons.at(index).rcsAmountXMax;
        rcsAmountXSpeed = Settings::Aimbot::weapons.at(index).rcsAmountXSpeed;
        rcsAmountYMin = Settings::Aimbot::weapons.at(index).rcsAmountYMin;
        rcsAmountYMax = Settings::Aimbot::weapons.at(index).rcsAmountYMax;
        rcsAmountYSpeed = Settings::Aimbot::weapons.at(index).rcsAmountYSpeed;
        autoPistolEnabled = Settings::Aimbot::weapons.at(index).autoPistolEnabled;
        autoShootEnabled = Settings::Aimbot::weapons.at(index).autoShootEnabled;
        autoScopeEnabled = Settings::Aimbot::weapons.at(index).autoScopeEnabled;
        noShootEnabled = Settings::Aimbot::weapons.at(index).noShootEnabled;
        ignoreJumpEnabled = Settings::Aimbot::weapons.at(index).ignoreJumpEnabled;
	ignoreEnemyJumpEnabled = Settings::Aimbot::weapons.at(index).ignoreEnemyJumpEnabled;
        smokeCheck = Settings::Aimbot::weapons.at(index).smokeCheck;
        flashCheck = Settings::Aimbot::weapons.at(index).flashCheck;
        spreadLimitEnabled = Settings::Aimbot::weapons.at(index).spreadLimitEnabled;
        spreadLimit = Settings::Aimbot::weapons.at(index).spreadLimit;
        autoWallEnabled = Settings::Aimbot::weapons.at(index).autoWallEnabled;
        autoWallValue = Settings::Aimbot::weapons.at(index).autoWallValue;
        autoAimRealDistance = Settings::Aimbot::weapons.at(index).autoAimRealDistance;
        autoSlow = Settings::Aimbot::weapons.at(index).autoSlow;
        predEnabled = Settings::Aimbot::weapons.at(index).predEnabled;

	curveValue[0] = Settings::Aimbot::weapons.at(index).curveValue[0];
	curveValue[1] = Settings::Aimbot::weapons.at(index).curveValue[1];
	curveValue[2] = Settings::Aimbot::weapons.at(index).curveValue[2];
	curveValue[3] = Settings::Aimbot::weapons.at(index).curveValue[3];
        for (int bone = (int) DesiredBones::BONE_PELVIS; bone <= (int) DesiredBones::BONE_RIGHT_SOLE; bone++)
                desiredBones[bone] = Settings::Aimbot::weapons.at(index).desiredBones[bone];
}

void UI::UpdateWeaponSettings()
{
        if (Settings::Aimbot::weapons.find(currentWeapon) == Settings::Aimbot::weapons.end())
                Settings::Aimbot::weapons[currentWeapon] = AimbotWeapon_t();

        AimbotWeapon_t settings = {
                .enabled = enabled,
                .silent = silent,
                .friendly = friendly,
                .closestBone = closestBone,
                .engageLock = engageLock,
                .engageLockTR = engageLockTR,
                .aimkeyOnly = aimkeyOnly,
                .smoothEnabled = smoothEnabled,
                .smoothSaltEnabled = smoothSaltEnabled,
                .errorMarginEnabled = errorMarginEnabled,
		.curveEnabled = curveEnabled,
                .autoAimEnabled = autoAimEnabled,
                .aimStepEnabled = aimStepEnabled,
                .rcsEnabled = rcsEnabled,
                .rcsAlwaysOn = rcsAlwaysOn,
                .spreadLimitEnabled = spreadLimitEnabled,
                .autoPistolEnabled = autoPistolEnabled,
                .autoShootEnabled = autoShootEnabled,
                .autoScopeEnabled = autoScopeEnabled,
                .noShootEnabled = noShootEnabled,
                .ignoreJumpEnabled = ignoreJumpEnabled,
		.ignoreEnemyJumpEnabled = ignoreEnemyJumpEnabled,

                .smokeCheck = smokeCheck,
                .flashCheck = flashCheck,
                .autoWallEnabled = autoWallEnabled,
                .autoAimRealDistance = autoAimRealDistance,
                .autoSlow = autoSlow,
                .predEnabled = predEnabled,

                .engageLockTTR = engageLockTTR,
                .bone = bone,
                .smoothType = smoothType,
                .aimkey = aimkey,
                .smoothAmount = smoothValue,
                .smoothSaltMultiplier = smoothSaltMultiplier,
                .errorMarginValue = errorMarginValue,
                .autoAimFov = autoAimValue,
                .aimStepMin = aimStepMin,
                .aimStepMax = aimStepMax,
                .rcsAmountXMin = rcsAmountXMin,
                .rcsAmountXMax = rcsAmountXMax,
                .rcsAmountXSpeed = rcsAmountXSpeed,
                .rcsAmountYMin = rcsAmountYMin,
                .rcsAmountYMax = rcsAmountYMax,
                .rcsAmountYSpeed = rcsAmountYSpeed,
                .autoWallValue = autoWallValue,
                .spreadLimit = spreadLimit,
        };

	settings.curveValue[0] = curveValue[0];
	settings.curveValue[1] = curveValue[1];
	settings.curveValue[2] = curveValue[2];
	settings.curveValue[3] = curveValue[3];

        for (int bone = (int) DesiredBones::BONE_PELVIS; bone <= (int) DesiredBones::BONE_RIGHT_SOLE; bone++)
                settings.desiredBones[bone] = desiredBones[bone];

        Settings::Aimbot::weapons.at(currentWeapon) = settings;

        if (Settings::Aimbot::weapons.at(currentWeapon) == Settings::Aimbot::weapons.at(ItemDefinitionIndex::INVALID) &&
            currentWeapon != ItemDefinitionIndex::INVALID)
        {
                Settings::Aimbot::weapons.erase(currentWeapon);
                UI::ReloadWeaponSettings();
        }
}

void Aimbot::RenderTab()
{
        const char* targets[] = { "PELVIS", "", "", "HIP", "LOWER SPINE", "MIDDLE SPINE", "UPPER SPINE", "NECK", "HEAD" };
        const char* smoothTypes[] = { "Slow Near End", "Constant Speed", "Fast Near End" };
        static char filterWeapons[32];

        if (ImGui::Checkbox(XORSTR("Enabled"), &enabled))
                UI::UpdateWeaponSettings();
        ImGui::Separator();

        ImGui::Columns(3, nullptr, false);
        {
                ImGui::SetColumnOffset(1, 200);
                ImGui::PushItemWidth(-1);
                ImGui::InputText(XORSTR("##FILTERWEAPONS"), filterWeapons, IM_ARRAYSIZE(filterWeapons));
                ImGui::PopItemWidth();
                ImGui::ListBoxHeader(XORSTR("##GUNS"), ImVec2(-1, -1));
                for (auto it : ItemDefinitionIndexMap)
                {
                        bool isDefault = (int) it.first < 0;
                        if (!isDefault && !Util::Contains(Util::ToLower(std::string(filterWeapons)), Util::ToLower(Util::Items::GetItemDisplayName(it.first).c_str())))
                                continue;

                        if (Util::Items::IsKnife(it.first) || Util::Items::IsGlove(it.first) || Util::Items::IsUtility(it.first))
                                continue;

                        const bool item_selected = ((int) it.first == (int) currentWeapon);
                        ImGui::PushID((int) it.first);
                        std::string formattedName;
                        char changeIndicator = ' ';
                        bool isChanged = Settings::Aimbot::weapons.find(it.first) != Settings::Aimbot::weapons.end();
                        if (!isDefault && isChanged)
                                changeIndicator = '*';
                        formattedName = changeIndicator + (isDefault ? Util::Items::GetItemDisplayName(it.first).c_str() : Util::Items::GetItemDisplayName(it.first));
                        if (ImGui::Selectable(formattedName.c_str(), item_selected))
                        {
                                currentWeapon = it.first;
                                UI::ReloadWeaponSettings();
                        }
                        ImGui::PopID();
                }
                ImGui::ListBoxFooter();
        }
        ImGui::NextColumn();
        {
                ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 75);
                ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 0), true);
                {
                        ImGui::Text(XORSTR("Target"));
                        ImGui::Separator();
                        ImGui::Columns(2, nullptr, false);
                        {
                                if (ImGui::Checkbox(XORSTR("FriendlyFire"), &friendly))
                                        UI::UpdateWeaponSettings();
                        }
                        ImGui::NextColumn();
                        {
                                ImGui::PushItemWidth(-1);
                                ImGui::Text(XORSTR("Aimbot Target"));
                                if(!closestBone){
                                        if (ImGui::Combo(XORSTR("##AIMTARGET"), (int*)&bone, targets, IM_ARRAYSIZE(targets)))
                                                UI::UpdateWeaponSettings();
                                }
                                if( closestBone )
                                {
                                        if(ImGui::Button(XORSTR("Bones"), ImVec2(-1, 0)))
                                                ImGui::OpenPopup(XORSTR("optionBones"));
                                        ImGui::SetNextWindowSize(ImVec2((ImGui::GetWindowWidth()/1.25f),ImGui::GetWindowHeight()), ImGuiSetCond_Always);
                                        if( ImGui::BeginPopup(XORSTR("optionBones")) )
                                        {
                                                ImGui::PushItemWidth(-1);
                                                ImGui::Text(XORSTR("Center Mass"));
                                                if( ImGui::Checkbox(XORSTR("Head"), &desiredBones[(int)DesiredBones::BONE_HEAD]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Neck"), &desiredBones[(int)DesiredBones::BONE_NECK]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Upper Spine"), &desiredBones[(int)DesiredBones::BONE_UPPER_SPINAL_COLUMN]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Middle Spine"), &desiredBones[(int)DesiredBones::BONE_MIDDLE_SPINAL_COLUMN]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Lower Spine"), &desiredBones[(int)DesiredBones::BONE_LOWER_SPINAL_COLUMN]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Pelvis"), &desiredBones[(int)DesiredBones::BONE_PELVIS]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Hip"), &desiredBones[(int)DesiredBones::BONE_HIP]) )
                                                        UI::UpdateWeaponSettings();
                                                ImGui::Separator();

                                                ImGui::Columns(2, nullptr, false);
                                                {
                                                        ImGui::Text(XORSTR("Player's Right Arm"));
                                                        if( ImGui::Checkbox(XORSTR("Collarbone"), &desiredBones[(int)DesiredBones::BONE_RIGHT_COLLARBONE]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Shoulder"), &desiredBones[(int)DesiredBones::BONE_RIGHT_SHOULDER]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Armpit"), &desiredBones[(int)DesiredBones::BONE_RIGHT_ARMPIT]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Bicep"), &desiredBones[(int)DesiredBones::BONE_RIGHT_BICEP]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Elbow"), &desiredBones[(int)DesiredBones::BONE_RIGHT_ELBOW]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Forearm"), &desiredBones[(int)DesiredBones::BONE_RIGHT_FOREARM]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Wrist"), &desiredBones[(int)DesiredBones::BONE_RIGHT_WRIST]) )
                                                                UI::UpdateWeaponSettings();
                                                        ImGui::Text(XORSTR("Player's Right Leg"));
                                                        if( ImGui::Checkbox(XORSTR("Buttcheek"), &desiredBones[(int)DesiredBones::BONE_RIGHT_BUTTCHEEK]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Thigh"), &desiredBones[(int)DesiredBones::BONE_RIGHT_THIGH]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Knee"), &desiredBones[(int)DesiredBones::BONE_RIGHT_KNEE]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Ankle"), &desiredBones[(int)DesiredBones::BONE_RIGHT_ANKLE]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Sole"), &desiredBones[(int)DesiredBones::BONE_RIGHT_SOLE]) )
                                                                UI::UpdateWeaponSettings();
                                                }
                                                ImGui::NextColumn();
                                                {   // these spaces are here in the strings because checkboxes can't have duplicate titles.
                                                        ImGui::Text(XORSTR("Player's Left Arm"));
                                                        if( ImGui::Checkbox(XORSTR("Collarbone "), &desiredBones[(int)DesiredBones::BONE_LEFT_COLLARBONE]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Shoulder "), &desiredBones[(int)DesiredBones::BONE_LEFT_SHOULDER]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Armpit "), &desiredBones[(int)DesiredBones::BONE_LEFT_ARMPIT]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Bicep "), &desiredBones[(int)DesiredBones::BONE_LEFT_BICEP]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Elbow "), &desiredBones[(int)DesiredBones::BONE_LEFT_ELBOW]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Forearm "), &desiredBones[(int)DesiredBones::BONE_LEFT_FOREARM]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Wrist "), &desiredBones[(int)DesiredBones::BONE_LEFT_WRIST]) )
                                                                UI::UpdateWeaponSettings();

                                                        ImGui::Text(XORSTR("Player's Left Leg"));
                                                        if( ImGui::Checkbox(XORSTR("Buttcheek "), &desiredBones[(int)DesiredBones::BONE_LEFT_BUTTCHEEK]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Thigh "), &desiredBones[(int)DesiredBones::BONE_LEFT_THIGH]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Knee "), &desiredBones[(int)DesiredBones::BONE_LEFT_KNEE]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Ankle "), &desiredBones[(int)DesiredBones::BONE_LEFT_ANKLE]) )
                                                                UI::UpdateWeaponSettings();
                                                        if( ImGui::Checkbox(XORSTR("Sole "), &desiredBones[(int)DesiredBones::BONE_LEFT_SOLE]) )
                                                                UI::UpdateWeaponSettings();
                                                }
                                                ImGui::PopItemWidth();
                                                ImGui::EndPopup();
                                        }
                                }
                                if(ImGui::Checkbox(XORSTR("ClosestBone"), &closestBone))
                                        UI::UpdateWeaponSettings();

                                if(ImGui::Checkbox(XORSTR("EngageLock"), &engageLock))
                                        UI::UpdateWeaponSettings();
                                if(engageLock)
                                {
                                        if(ImGui::Checkbox(XORSTR("Target Reacquisition"), &engageLockTR))
                                                UI::UpdateWeaponSettings();
                                        if( engageLockTR )
                                        {
                                                if(ImGui::SliderInt(XORSTR("##TTR"), &engageLockTTR, 0, 1000))
                                                        UI::UpdateWeaponSettings();
                                        }
                                }
                                ImGui::PopItemWidth();
                        }
                        ImGui::Columns(1);
                        ImGui::Separator();
                        ImGui::Text(XORSTR("Accuracy"));
                        ImGui::Separator();
                        ImGui::Columns(2, nullptr, false);
                        {
                                if (ImGui::Checkbox(XORSTR("Auto Aim"), &autoAimEnabled))
                                        UI::UpdateWeaponSettings();
                                if (ImGui::Checkbox(XORSTR("Distance-Based FOV"), &autoAimRealDistance))
                                        UI::UpdateWeaponSettings();
                        }
                        ImGui::NextColumn();
                        {
                                ImGui::PushItemWidth(-1);
                                if (ImGui::SliderFloat(XORSTR("##AA"), &autoAimValue, 0, 180))
                                        UI::UpdateWeaponSettings();
                                ImGui::PopItemWidth();
                        }
                        ImGui::Columns(1);
                        ImGui::Separator();
                        ImGui::Text(XORSTR("Recoil Control"));
                        ImGui::Separator();
                        ImGui::Columns(2, nullptr, false);
                        {
                                if (ImGui::Checkbox(XORSTR("Enabled"), &rcsEnabled))
                                        UI::UpdateWeaponSettings();

				ImGui::Text(XORSTR("Pitch"));
                                ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat(XORSTR("##RCSXMIN"), &rcsAmountXMin, 0, rcsAmountXMax, XORSTR("Min Pitch: %0.3f")))
					UI::UpdateWeaponSettings();
				if (ImGui::SliderFloat(XORSTR("##RCSXMAX"), &rcsAmountXMax, rcsAmountXMin, 2, XORSTR("Max Pitch: %0.3f")))
					UI::UpdateWeaponSettings();
				if (ImGui::SliderFloat(XORSTR("##RCSXSPEED"), &rcsAmountXSpeed, 0, 1, XORSTR("Speed: %0.3f")))
					UI::UpdateWeaponSettings();
                                ImGui::PopItemWidth();
                        }
                        ImGui::NextColumn();
                        {
				if (ImGui::Checkbox(XORSTR("RCS Always on"), &rcsAlwaysOn))
					UI::UpdateWeaponSettings();

				ImGui::Text(XORSTR("Yaw"));
                                ImGui::PushItemWidth(-1);
				if (ImGui::SliderFloat(XORSTR("##RCSYMIN"), &rcsAmountYMin, 0, rcsAmountYMax, XORSTR("Min Yaw: %0.3f")))
					UI::UpdateWeaponSettings();
				if (ImGui::SliderFloat(XORSTR("##RCSYMAX"), &rcsAmountYMax, rcsAmountYMin, 2, XORSTR("Max Yaw: %0.3f")))
					UI::UpdateWeaponSettings();
				if (ImGui::SliderFloat(XORSTR("##RCSYSPEED"), &rcsAmountYSpeed, 0, 1, XORSTR("Speed: %0.3f")))
					UI::UpdateWeaponSettings();
                                ImGui::PopItemWidth();
                        }
                        ImGui::Columns(1);
                        ImGui::Separator();
                        ImGui::Text(XORSTR("Humanizing"));
                        ImGui::Separator();
                        ImGui::Columns(2, nullptr, false);
                        {
                                if (ImGui::Checkbox(XORSTR("Smoothing"), &smoothEnabled))
                                        UI::UpdateWeaponSettings();
                                if (ImGui::Checkbox(XORSTR("Smooth Salting"), &smoothSaltEnabled))
                                        UI::UpdateWeaponSettings();
                                if (ImGui::Checkbox(XORSTR("Error Margin"), &errorMarginEnabled))
                                        UI::UpdateWeaponSettings();
                                ImGui::PushItemWidth(-1);
                                if (ImGui::Combo(XORSTR("##SMOOTHTYPE"), (int*)& smoothType, smoothTypes, IM_ARRAYSIZE(smoothTypes)))
                                        UI::UpdateWeaponSettings();
                                ImGui::PopItemWidth();
                        }
                        ImGui::NextColumn();
                        {
                                ImGui::PushItemWidth(-1);
                                if (ImGui::SliderFloat(XORSTR("##SMOOTH"), &smoothValue, 0, 1))
                                        UI::UpdateWeaponSettings();
                                if (ImGui::SliderFloat(XORSTR("##SALT"), &smoothSaltMultiplier, 0, smoothValue))
                                        UI::UpdateWeaponSettings();
                                if (ImGui::SliderFloat(XORSTR("##ERROR"), &errorMarginValue, 0, 2))
                                        UI::UpdateWeaponSettings();
                                ImGui::PopItemWidth();
                        }
			ImGui::EndColumns();
			{
				if (ImGui::Checkbox(XORSTR("Enabled##CURVE"), &curveEnabled))
                                        UI::UpdateWeaponSettings();
				if (ImGui::Bezier( XORSTR("Curve"), curveValue, false, false ))
                                        UI::UpdateWeaponSettings();
			}
			
                        ImGui::EndChild();
                }
        }
        ImGui::NextColumn();
        {
                ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, 0), true);
                {
                        ImGui::Text(XORSTR("Aimkey Only"));
                        ImGui::Separator();
                        ImGui::Columns(2, nullptr, false);
                        {
                                if (ImGui::Checkbox(XORSTR("Enabled"), &aimkeyOnly))
                                        UI::UpdateWeaponSettings();
                        }
                        ImGui::NextColumn();
                        {
                                UI::KeyBindButton(&aimkey);
                        }
                        ImGui::Columns(1);
                        ImGui::Separator();
                        ImGui::Text(XORSTR("Casual / DM Only"));
                        ImGui::Separator();
                        ImGui::Columns(2, nullptr, false);
                        {
                                if (ImGui::Checkbox(XORSTR("Aim Step"), &aimStepEnabled))
                                        UI::UpdateWeaponSettings();
                        }
                        ImGui::NextColumn();
                        {
                                ImGui::PushItemWidth(-1);
                                ImGui::Text(XORSTR("Min"));
                                if (ImGui::SliderFloat(XORSTR("##STEPMIN"), &aimStepMin, 5, 35))
                                        UI::UpdateWeaponSettings();
                                ImGui::Text(XORSTR("Max"));
                                if (ImGui::SliderFloat(XORSTR("##STEPMAX"), &aimStepMax, (aimStepMin) + 1.0f, 35))
                                        UI::UpdateWeaponSettings();
                                ImGui::PopItemWidth();
                        }
                        ImGui::Columns(1);
                        ImGui::Separator();
                        ImGui::Text(XORSTR("Other"));
                        ImGui::Separator();
                        ImGui::Columns(2, nullptr, false);
                        {

                                switch (currentWeapon)
                                {
                                case ItemDefinitionIndex::INVALID:
                                case ItemDefinitionIndex::WEAPON_DEAGLE:
                                case ItemDefinitionIndex::WEAPON_ELITE:
                                case ItemDefinitionIndex::WEAPON_FIVESEVEN:
                                case ItemDefinitionIndex::WEAPON_GLOCK:
                                case ItemDefinitionIndex::WEAPON_TEC9:
                                case ItemDefinitionIndex::WEAPON_HKP2000:
                                case ItemDefinitionIndex::WEAPON_USP_SILENCER:
                                case ItemDefinitionIndex::WEAPON_P250:
                                case ItemDefinitionIndex::WEAPON_CZ75A:
                                case ItemDefinitionIndex::WEAPON_REVOLVER:
                                        if (ImGui::Checkbox(XORSTR("Auto Pistol"), &autoPistolEnabled))
                                                UI::UpdateWeaponSettings();
                                        break;
                                default:
                                        break;
                                }

                                if (ImGui::Checkbox(XORSTR("Silent Aim"), &silent))
                                        UI::UpdateWeaponSettings();
                                if (ImGui::Checkbox(XORSTR("Smoke Check"), &smokeCheck))
                                        UI::UpdateWeaponSettings();
                                if (ImGui::Checkbox(XORSTR("Prediction"), &predEnabled))
                                        UI::UpdateWeaponSettings();
                        }
			ImGui::NextColumn();
			{
				if (ImGui::Checkbox(XORSTR("No Shoot"), &noShootEnabled))
					UI::UpdateWeaponSettings();
				switch (currentWeapon)
				{
					case ItemDefinitionIndex::WEAPON_DEAGLE:
					case ItemDefinitionIndex::WEAPON_ELITE:
					case ItemDefinitionIndex::WEAPON_FIVESEVEN:
					case ItemDefinitionIndex::WEAPON_GLOCK:
					case ItemDefinitionIndex::WEAPON_TEC9:
					case ItemDefinitionIndex::WEAPON_HKP2000:
					case ItemDefinitionIndex::WEAPON_USP_SILENCER:
					case ItemDefinitionIndex::WEAPON_P250:
					case ItemDefinitionIndex::WEAPON_CZ75A:
					case ItemDefinitionIndex::WEAPON_REVOLVER:
						break;
					default:
						if (ImGui::Checkbox(XORSTR("Auto Scope"), &autoScopeEnabled))
							UI::UpdateWeaponSettings();
				}

				if (ImGui::Checkbox(XORSTR("Flash Check"), &flashCheck))
					UI::UpdateWeaponSettings();
				if (ImGui::Checkbox(XORSTR("Ignore Jump (Self)"), &ignoreJumpEnabled))
					UI::UpdateWeaponSettings();
				if (ImGui::Checkbox(XORSTR("Ignore Jump (Enemies)"), &ignoreEnemyJumpEnabled))
					UI::UpdateWeaponSettings();
			}


                        ImGui::Columns(1);
                        ImGui::Separator();
                        ImGui::Text(XORSTR("AutoSlow"));
                        ImGui::Separator();
                        if (ImGui::Checkbox(XORSTR("Enabled##AUTOSLOW"), &autoSlow))
                                UI::UpdateWeaponSettings();

                        ImGui::Columns(1);
                        ImGui::Separator();
                        ImGui::Text(XORSTR("AutoWall"));
                        ImGui::Separator();
                        ImGui::Columns(2, nullptr, false);
                        {
                                if (ImGui::Checkbox(XORSTR("Enabled##AUTOWALL"), &autoWallEnabled))
                                        UI::UpdateWeaponSettings();
                                if(ImGui::Button(XORSTR("Autowall Bones"), ImVec2(-1, 0)))
                                        ImGui::OpenPopup(XORSTR("optionBones"));
                                ImGui::SetNextWindowSize(ImVec2((ImGui::GetWindowWidth()/1.25f),ImGui::GetWindowHeight()), ImGuiSetCond_Always);
                                if( ImGui::BeginPopup(XORSTR("optionBones")) )
                                {
                                        ImGui::PushItemWidth(-1);
                                        ImGui::Text(XORSTR("Center Mass"));
                                        if( ImGui::Checkbox(XORSTR("Head"), &desiredBones[(int)DesiredBones::BONE_HEAD]) )
                                                UI::UpdateWeaponSettings();
                                        if( ImGui::Checkbox(XORSTR("Neck"), &desiredBones[(int)DesiredBones::BONE_NECK]) )
                                                UI::UpdateWeaponSettings();
                                        if( ImGui::Checkbox(XORSTR("Upper Spine"), &desiredBones[(int)DesiredBones::BONE_UPPER_SPINAL_COLUMN]) )
                                                UI::UpdateWeaponSettings();
                                        if( ImGui::Checkbox(XORSTR("Middle Spine"), &desiredBones[(int)DesiredBones::BONE_MIDDLE_SPINAL_COLUMN]) )
                                                UI::UpdateWeaponSettings();
                                        if( ImGui::Checkbox(XORSTR("Lower Spine"), &desiredBones[(int)DesiredBones::BONE_LOWER_SPINAL_COLUMN]) )
                                                UI::UpdateWeaponSettings();
                                        if( ImGui::Checkbox(XORSTR("Pelvis"), &desiredBones[(int)DesiredBones::BONE_PELVIS]) )
                                                UI::UpdateWeaponSettings();
                                        if( ImGui::Checkbox(XORSTR("Hip"), &desiredBones[(int)DesiredBones::BONE_HIP]) )
                                                UI::UpdateWeaponSettings();
                                        ImGui::Separator();

                                        ImGui::Columns(2, nullptr, false);
                                        {
                                                ImGui::Text(XORSTR("Player's Right Arm"));
                                                if( ImGui::Checkbox(XORSTR("Collarbone"), &desiredBones[(int)DesiredBones::BONE_RIGHT_COLLARBONE]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Shoulder"), &desiredBones[(int)DesiredBones::BONE_RIGHT_SHOULDER]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Armpit"), &desiredBones[(int)DesiredBones::BONE_RIGHT_ARMPIT]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Bicep"), &desiredBones[(int)DesiredBones::BONE_RIGHT_BICEP]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Elbow"), &desiredBones[(int)DesiredBones::BONE_RIGHT_ELBOW]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Forearm"), &desiredBones[(int)DesiredBones::BONE_RIGHT_FOREARM]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Wrist"), &desiredBones[(int)DesiredBones::BONE_RIGHT_WRIST]) )
                                                        UI::UpdateWeaponSettings();
                                                ImGui::Text(XORSTR("Player's Right Leg"));
                                                if( ImGui::Checkbox(XORSTR("Buttcheek"), &desiredBones[(int)DesiredBones::BONE_RIGHT_BUTTCHEEK]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Thigh"), &desiredBones[(int)DesiredBones::BONE_RIGHT_THIGH]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Knee"), &desiredBones[(int)DesiredBones::BONE_RIGHT_KNEE]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Ankle"), &desiredBones[(int)DesiredBones::BONE_RIGHT_ANKLE]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Sole"), &desiredBones[(int)DesiredBones::BONE_RIGHT_SOLE]) )
                                                        UI::UpdateWeaponSettings();
                                        }
                                        ImGui::NextColumn();
                                        {
                                                ImGui::Text(XORSTR("Player's Left Arm"));
                                                if( ImGui::Checkbox(XORSTR("Collarbone "), &desiredBones[(int)DesiredBones::BONE_LEFT_COLLARBONE]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Shoulder "), &desiredBones[(int)DesiredBones::BONE_LEFT_SHOULDER]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Armpit "), &desiredBones[(int)DesiredBones::BONE_LEFT_ARMPIT]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Bicep "), &desiredBones[(int)DesiredBones::BONE_LEFT_BICEP]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Elbow "), &desiredBones[(int)DesiredBones::BONE_LEFT_ELBOW]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Forearm "), &desiredBones[(int)DesiredBones::BONE_LEFT_FOREARM]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Wrist "), &desiredBones[(int)DesiredBones::BONE_LEFT_WRIST]) )
                                                        UI::UpdateWeaponSettings();

                                                ImGui::Text(XORSTR("Player's Left Leg"));
                                                if( ImGui::Checkbox(XORSTR("Buttcheek "), &desiredBones[(int)DesiredBones::BONE_LEFT_BUTTCHEEK]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Thigh "), &desiredBones[(int)DesiredBones::BONE_LEFT_THIGH]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Knee "), &desiredBones[(int)DesiredBones::BONE_LEFT_KNEE]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Ankle "), &desiredBones[(int)DesiredBones::BONE_LEFT_ANKLE]) )
                                                        UI::UpdateWeaponSettings();
                                                if( ImGui::Checkbox(XORSTR("Sole "), &desiredBones[(int)DesiredBones::BONE_LEFT_SOLE]) )
                                                        UI::UpdateWeaponSettings();
                                        }
                                        ImGui::PopItemWidth();
                                        ImGui::EndPopup();
                                }
                        }
                        ImGui::NextColumn();
                        {
                                ImGui::PushItemWidth(-1);
                                if (ImGui::SliderFloat(XORSTR("##AUTOWALLDMG"), &autoWallValue, 0, 100, XORSTR("Min Damage: %f")))
                                        UI::UpdateWeaponSettings();
                                ImGui::PopItemWidth();
                        }

                        ImGui::Columns(1);
                        ImGui::Separator();
                        ImGui::Text(XORSTR("Autoshoot"));
                        ImGui::Separator();
                        if (ImGui::Checkbox(XORSTR("Auto Shoot"), &autoShootEnabled))
                                UI::UpdateWeaponSettings();
                        ImGui::Checkbox(XORSTR("Velocity Check"), &Settings::Aimbot::AutoShoot::velocityCheck);
                        if( ImGui::Checkbox(XORSTR("Spread Limit"), &spreadLimitEnabled) )
                                UI::UpdateWeaponSettings();
                        if( ImGui::SliderFloat(XORSTR("##SPREADLIMIT"), &spreadLimit, 0, 0.1) )
                                UI::UpdateWeaponSettings();

                        ImGui::Columns(1);
                        if (currentWeapon > ItemDefinitionIndex::INVALID && Settings::Aimbot::weapons.find(currentWeapon) != Settings::Aimbot::weapons.end())
                        {
				ImGui::Separator();
                                if (ImGui::Button(XORSTR("Clear Weapon Settings"), ImVec2(-1, 0)))
                                {
                                        Settings::Aimbot::weapons.erase(currentWeapon);
                                        UI::ReloadWeaponSettings();
                                }
                        }
                        ImGui::EndChild();
                }
        }
}
