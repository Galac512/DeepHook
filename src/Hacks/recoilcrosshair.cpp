#include "recoilcrosshair.h"

#include "../Hooks/hooks.h"

#include "../interfaces.h"
#include "../settings.h"
#include "../Utils/draw.h"

bool Settings::Recoilcrosshair::enabled = false;
bool Settings::Recoilcrosshair::showOnlyWhenShooting = false;

void Recoilcrosshair::PaintHybrid( ) {
	if( !Settings::Recoilcrosshair::enabled )
		return;
	C_BasePlayer* localplayer = ( C_BasePlayer* ) entityList->GetClientEntity( engine->GetLocalPlayer() );
	if ( !localplayer || !localplayer->GetAlive() )
		return;

	if ( localplayer->GetShotsFired() < 1 && Settings::Recoilcrosshair::showOnlyWhenShooting )
		return;

	C_BaseCombatWeapon* activeWeapon = ( C_BaseCombatWeapon* ) entityList->GetClientEntityFromHandle(
			localplayer->GetActiveWeapon() );
	if ( !activeWeapon )
		return;

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if ( weaponType != CSWeaponType::WEAPONTYPE_RIFLE && weaponType != CSWeaponType::WEAPONTYPE_SUBMACHINEGUN &&
		 weaponType != CSWeaponType::WEAPONTYPE_MACHINEGUN )
		return;

	QAngle punchAngle = *localplayer->GetAimPunchAngle();

	int screenWidth, screenHeight;
	Draw::HyGetScreenSize( &screenWidth, &screenHeight );

	int x = screenWidth / 2;
	int y = screenHeight / 2;
	int dx = screenWidth / OverrideView::currentFOV;
	int dy = screenHeight / OverrideView::currentFOV;

	int crosshairX = ( int ) ( x - ( dx * punchAngle.y ) );
	int crosshairY = ( int ) ( y + ( dy * punchAngle.x ) );

	// outline horizontal
	Draw::HyFilledRectangle( crosshairX - 4, crosshairY - 1, crosshairX + 5, crosshairY + 2, ImColor( 0, 0, 0, 225 ) );
	// outline vertical
	Draw::HyFilledRectangle( crosshairX - 1, crosshairY - 4, crosshairX + 2, crosshairY + 5, ImColor( 0, 0, 0, 225 ) );
	// line horizontal
	Draw::HyLine( crosshairX - 3, crosshairY, crosshairX + 4, crosshairY, ImColor( 255, 255, 255, 255 ) );
	// line vertical
	Draw::HyLine( crosshairX, crosshairY + 3, crosshairX, crosshairY - 4, ImColor( 255, 255, 255, 255 ) );
}

