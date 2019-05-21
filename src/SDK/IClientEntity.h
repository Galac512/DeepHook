#pragma once

#include "vector.h"
#include "common.h"
#include "../offsets.h"
#include "definitions.h"
#include "CCSGOAnimState.h"
#include "CUtlVector.h"

#define MAX_SHOOT_SOUNDS 16
#define MAX_WEAPON_STRING 80
#define MAX_WEAPON_PREFIX 16
#define MAX_WEAPON_AMMO_NAME 32


class model_t;
class ClientClass;

typedef int (* GetSequenceActivityFn)( void*, int ); // C_BaseAnimating::GetSequenceActivity(int sequence).
extern GetSequenceActivityFn GetSeqActivity;
extern uintptr_t SetAbsOriginFnAddr;

enum WeaponSound_t
{
	EMPTY,
	SINGLE,
	SINGLE_NPC,
	WPN_DOUBLE, // Can't be "DOUBLE" because windows.h uses it.
	DOUBLE_NPC,
	BURST,
	RELOAD,
	RELOAD_NPC,
	MELEE_MISS,
	MELEE_HIT,
	MELEE_HIT_WORLD,
	SPECIAL1,
	SPECIAL2,
	SPECIAL3,
	TAUNT,
	FAST_RELOAD,

	// Add new shoot sound types here
	REVERSE_THE_NEW_SOUND,

	NUM_SHOOT_SOUND_TYPES,
};

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

class AnimationLayer
{
public:
    char pad_0000[24];
    int m_nOrder;
    int m_nSequence; // 0x1C
    float_t m_flPrevCycle;
    float_t m_flWeight;
    float_t m_flWeightDeltaRate;
    float_t m_flPlaybackRate;
    float_t m_flCycle;
    void *m_pOwner; // 0x38 // player's thisptr
    char pad_0038[8]; // 0x40
}; //Size: 0x0048

class ICollideable
{
public:
	virtual void pad0();
	virtual const Vector& OBBMins() const;
	virtual const Vector& OBBMaxs() const;
};

class IHandleEntity
{
public:
	virtual ~IHandleEntity() {};
};

class IClientUnknown : public IHandleEntity {};
class IClientRenderable
{
public:
	virtual ~IClientRenderable() {};

	model_t* GetModel()
	{
		typedef model_t* (* oGetModel)(void*);
		return getvfunc<oGetModel>(this, 8)(this);
	}

	bool SetupBones(matrix3x4_t* pBoneMatrix, int nMaxBones, int nBoneMask, float flCurTime = 0)
	{
		typedef bool (* oSetupBones)(void*, matrix3x4_t*, int, int, float);
		return getvfunc<oSetupBones>(this, 13)(this, pBoneMatrix, nMaxBones, nBoneMask, flCurTime);
	}
};

class IClientNetworkable
{
public:
	virtual ~IClientNetworkable() {};

	void Release()
	{
		typedef void (* oRelease)(void*);
		return getvfunc<oRelease>(this, 1)(this);
	}

	ClientClass* GetClientClass()
	{
		typedef ClientClass* (* oGetClientClass)(void*);
		return getvfunc<oGetClientClass>(this, 2)(this);
	}

	void PreDataUpdate(DataUpdateType_t updateType)
	{
		typedef void (* oPreDataUpdate)(void*, DataUpdateType_t);
		return getvfunc<oPreDataUpdate>(this, 6)(this, updateType);
	}

	bool GetDormant()
	{
		typedef bool (* oGetDormant)(void*);
		return getvfunc<oGetDormant>(this, 9)(this);
	}

	int GetIndex()
	{
		typedef int (* oGetIndex)(void*);
		return getvfunc<oGetIndex>(this, 10)(this);
	}

	void SetDestroyedOnRecreateEntities()
	{
		typedef void (* oSetDestroyedOnRecreateEntities)(void*);
		return getvfunc<oSetDestroyedOnRecreateEntities>(this, 13)(this);
	}
};

class IClientThinkable
{
public:
	virtual ~IClientThinkable() {};
};

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual ~IClientEntity() {};
};

class C_BaseEntity : public IClientEntity
{
public:

	IClientNetworkable* GetNetworkable()
	{
		return (IClientNetworkable*)((uintptr_t)this + 0x10);
	}

	const Vector& GetAbsOrigin( )
	{
		typedef const Vector& (* oGetAbsOrigin)(void*);
		return getvfunc<oGetAbsOrigin>( this, 12 )( this );
	}

	void SetAbsOrigin( const Vector * const angles ) {
		asm volatile ("mov %0, %%rdi;\n\t"
				"mov %1, %%rsi;\n\t"
				"call *%P2;"
		:
		:"r"(this), "r"(angles), "r"(SetAbsOriginFnAddr)
		:"%rdi", "%rsi"
		);
	}

	void SetModelIndex(int index)
	{
		typedef void (* oSetModelIndex)(void*, int);
		return getvfunc<oSetModelIndex>(this, 111)(this, index);
	}

	int* GetModelIndex()
	{
		return (int*)((uintptr_t)this + offsets.DT_BaseViewModel.m_nModelIndex);
	}

	float GetAnimTime()
	{
		return *(float*)((uintptr_t)this + offsets.DT_BaseEntity.m_flAnimTime);
	}

	float GetSimulationTime()
	{
		return *(float*)((uintptr_t)this + offsets.DT_BaseEntity.m_flSimulationTime);
	}

	TeamID GetTeam()
	{
		return *(TeamID*)((uintptr_t)this + offsets.DT_BaseEntity.m_iTeamNum);
	}

	int GetSurvivalTeam()
	{
		return *(int*)((uintptr_t)this + offsets.DT_CSPlayer.m_nSurvivalTeam);
	}

	Vector GetVecOrigin()
	{
		return *(Vector*)((uintptr_t)this + offsets.DT_BaseEntity.m_vecOrigin);
	}

	MoveType_t GetMoveType()
	{
		return *(MoveType_t*)((uintptr_t)this + offsets.DT_BaseEntity.m_MoveType);
	}

	ICollideable* GetCollideable()
	{
		return (ICollideable*)((uintptr_t)this + offsets.DT_BaseEntity.m_Collision);
	}

	bool* GetSpotted()
	{
		return (bool*)((uintptr_t)this + offsets.DT_BaseEntity.m_bSpotted);
	}
};

/* generic game classes */
class C_BasePlayer : public C_BaseEntity
{
public:
    int GetSequenceActivity(int sequence)
    {
        if( !GetSeqActivity )
            return -1;
        return GetSeqActivity( this, sequence );
    }
    CUtlVector<AnimationLayer>* GetAnimOverlay() {
        return reinterpret_cast<CUtlVector<AnimationLayer>*>((uintptr_t)this + Offsets::playerAnimOverlayOffset);
    }

    CCSGOAnimState* GetAnimState()
    {
        return *reinterpret_cast<CCSGOAnimState**>((uintptr_t)this + Offsets::playerAnimStateOffset);
    }

	QAngle* GetViewPunchAngle()
	{
		return (QAngle*)((uintptr_t)this + offsets.DT_BasePlayer.m_viewPunchAngle);
	}

	QAngle* GetAimPunchAngle()
	{
		return (QAngle*)((uintptr_t)this + offsets.DT_BasePlayer.m_aimPunchAngle);
	}

	Vector GetVecViewOffset()
	{
		return *(Vector*)((uintptr_t)this + offsets.DT_BasePlayer.m_vecViewOffset);
	}

	unsigned int GetTickBase()
	{
		return *(unsigned int*)((uintptr_t)this + offsets.DT_BasePlayer.m_nTickBase);
	}

	Vector GetVelocity()
	{
		return *(Vector*)((uintptr_t)this + offsets.DT_BasePlayer.m_vecVelocity);
	}

	int GetHealth()
	{
		return *(int*)((uintptr_t)this + offsets.DT_BasePlayer.m_iHealth);
	}

	unsigned char GetLifeState()
	{
		return *(unsigned char*)((uintptr_t)this + offsets.DT_BasePlayer.m_lifeState);
	}

	bool IsAlive()
	{
		return GetHealth() > 0 && GetLifeState() == 0;
	}

	int GetFlags()
	{
		return *(int*)((uintptr_t)this + offsets.DT_BasePlayer.m_fFlags);
	}

	ObserverMode_t* GetObserverMode()
	{
		return (ObserverMode_t*)((uintptr_t)this + offsets.DT_BasePlayer.m_iObserverMode);
	}

	void* GetObserverTarget()
	{
		return (void*)((uintptr_t)this + offsets.DT_BasePlayer.m_hObserverTarget);
	}

	void* GetViewModel()
	{
		return (void*)((uintptr_t)this + offsets.DT_BasePlayer.m_hViewModel);
	}

	const char* GetLastPlaceName()
	{
		return (const char*)((uintptr_t)this + offsets.DT_BasePlayer.m_szLastPlaceName);
	}

	int GetShotsFired()
	{
		return *(int*)((uintptr_t)this + offsets.DT_CSPlayer.m_iShotsFired);
	}

	QAngle* GetEyeAngles()
	{
		return (QAngle*)((uintptr_t)this + offsets.DT_CSPlayer.m_angEyeAngles[0]);
	}

	int GetMoney()
	{
		return *(int*)((uintptr_t)this + offsets.DT_CSPlayer.m_iAccount);
	}

	int GetHits()
	{
		return *(int*)((uintptr_t)this + offsets.DT_CSPlayer.m_totalHitsOnServer);
	}

	int GetArmor()
	{
		return *(int*)((uintptr_t)this + offsets.DT_CSPlayer.m_ArmorValue);
	}

	int HasDefuser()
	{
		return *(int*)((uintptr_t)this + offsets.DT_CSPlayer.m_bHasDefuser);
	}

	bool IsDefusing()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_CSPlayer.m_bIsDefusing);
	}

	bool IsGrabbingHostage()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_CSPlayer.m_bIsGrabbingHostage);
	}

	bool IsScoped()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_CSPlayer.m_bIsScoped);
	}

	bool GetImmune()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_CSPlayer.m_bGunGameImmunity);
	}

	bool IsRescuing()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_CSPlayer.m_bIsRescuing);
	}

	int HasHelmet()
	{
		return *(int*)((uintptr_t)this + offsets.DT_CSPlayer.m_bHasHelmet);
	}

	bool IsFlashed() // Pasted from CSGOSimple.
	{ // GetFlashBangTime() - globalVars->curtime > 2.0f
		return *(float*)((uintptr_t)this->GetFlashMaxAlpha() - 0x8) > 200.0;
	}

	float GetFlashBangTime()
	{
		return *(float*)((uintptr_t)this + 0xABF4);
	}

	float GetFlashDuration()
	{
		return *(float*)((uintptr_t)this + offsets.DT_CSPlayer.m_flFlashDuration);
	}

	float* GetFlashMaxAlpha()
	{
		return (float*)((uintptr_t)this + offsets.DT_CSPlayer.m_flFlashMaxAlpha);
	}

	float* GetLowerBodyYawTarget()
	{
		return (float*)((uintptr_t)this + offsets.DT_CSPlayer.m_flLowerBodyYawTarget);
	}

	void* GetActiveWeapon()
	{
		return (void*)((uintptr_t)this + offsets.DT_BaseCombatCharacter.m_hActiveWeapon);
	}

	int* GetWeapons()
	{
		return (int*)((uintptr_t)this + offsets.DT_BaseCombatCharacter.m_hMyWeapons);
	}

	int* GetWearables()
	{
		return (int*)((uintptr_t)this + offsets.DT_BaseCombatCharacter.m_hMyWearables);
	}

	bool GetAlive()
	{
		return this->GetHealth() > 0 && this->GetLifeState() == LIFE_ALIVE;
	}

	Vector GetEyePosition()
	{
		return this->GetVecOrigin() + this->GetVecViewOffset();
	}

	inline Vector GetBonePosition(int boneIndex)
	{
		matrix3x4_t BoneMatrix[MAXSTUDIOBONES];

		if (!this->SetupBones(BoneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0))
			return this->GetVecOrigin();

		matrix3x4_t hitbox = BoneMatrix[boneIndex];

		return Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
	}
	/*
	inline Vector GetNoInterpBonePosition(int boneIndex)
	{
		matrix3x4_t BoneMatrix[MAXSTUDIOBONES];
		Vector backup = this->GetAbsOrigin();
		Vector noInterp = this->GetVecOrigin();
		this->SetAbsOrigin( &noInterp );
		if (!this->SetupBones(BoneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0)) {
			this->SetAbsOrigin( &backup );
			return this->GetAbsOrigin( );
		}

		matrix3x4_t hitbox = BoneMatrix[boneIndex];

		this->SetAbsOrigin( &backup );
		return Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
	}*/

	QAngle* GetVAngles()
	{
		return (QAngle*)((uintptr_t)this + offsets.DT_BasePlayer.deadflag + 0x4);
	}
};

class C_PlantedC4 : public C_BaseEntity
{
public:
	bool IsBombTicking()
	{
		return (bool)((uintptr_t)this + offsets.DT_PlantedC4.m_bBombTicking);
	}

	float GetBombTime()
	{
		return *(float*)((uintptr_t)this + offsets.DT_PlantedC4.m_flC4Blow);
	}

	bool IsBombDefused()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_PlantedC4.m_bBombDefused);
	}

	int GetBombDefuser()
	{
		return *(int*)((uintptr_t)this + offsets.DT_PlantedC4.m_hBombDefuser);
	}
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:

	ItemDefinitionIndex* GetItemDefinitionIndex()
	{
		return (ItemDefinitionIndex*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_iItemDefinitionIndex);
	}

	bool* GetInitialized()
	{
		return (bool*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_bInitialized);
	}
	
	int* GetItemIDHigh()
	{
		return (int*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_iItemIDHigh);
	}
	
	int* GetEntityQuality()
	{
		return (int*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_iEntityQuality);
	}

	char* GetCustomName()
	{
		return (char*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_szCustomName);
	}

	int* GetFallbackPaintKit()
	{
		return (int*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_nFallbackPaintKit);
	}

	int* GetFallbackSeed()
	{
		return (int*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_nFallbackSeed);
	}

	float* GetFallbackWear()
	{
		return (float*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_flFallbackWear);
	}

	int* GetFallbackStatTrak()
	{
		return (int*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_nFallbackStatTrak);
	}

	int* GetAccountID()
	{
		return (int*)((uintptr_t)this + offsets.DT_BaseAttributableItem.m_iAccountID);
	}
};

class C_BaseViewModel: public C_BaseEntity
{
public:
	int GetWeapon()
	{
		return *(int*)((uintptr_t)this + offsets.DT_BaseViewModel.m_hWeapon);
	}

	int GetOwner()
	{
		return *(int*)((uintptr_t)this + offsets.DT_BaseViewModel.m_hOwner);
	}
};

class CHudTexture;

class FileWeaponInfo_t
{
public:
	FileWeaponInfo_t();

	// Each game can override this to get whatever values it wants from the script.
	virtual void Parse(KeyValues *pKeyValuesData, const char *szWeaponName);

	bool bParsedScript;
	bool bLoadedHudElements;

	char szClassName[MAX_WEAPON_STRING];
	char szPrintName[MAX_WEAPON_STRING];

	char szViewModel[MAX_WEAPON_STRING];
	char szWorldModel[MAX_WEAPON_STRING];
	char szAmmo1[MAX_WEAPON_AMMO_NAME];
	char szWorldDroppedModel[MAX_WEAPON_STRING];
	char szAnimationPrefix[MAX_WEAPON_PREFIX];
	int iSlot;
	int iPosition;
	int iMaxClip1;
	int iMaxClip2;
	int iDefaultClip1;
	int iDefaultClip2;
	int iWeight;
	int iRumbleEffect;
	bool bAutoSwitchTo;
	bool bAutoSwitchFrom;
	int iFlags;
	char szAmmo2[MAX_WEAPON_AMMO_NAME];
	char szAIAddOn[MAX_WEAPON_STRING];

	// Sound blocks
	char aShootSounds[NUM_SHOOT_SOUND_TYPES][MAX_WEAPON_STRING];

	int iAmmoType;
	int iAmmo2Type;
	bool m_bMeleeWeapon;

	// This tells if the weapon was built right-handed (defaults to true).
	// This helps cl_righthand make the decision about whether to flip the model or not.
	bool m_bBuiltRightHanded;
	bool m_bAllowFlipping;

	// Sprite data, read from the data file
	int iSpriteCount;
	CHudTexture* iconActive;
	CHudTexture* iconInactive;
	CHudTexture* iconAmmo;
	CHudTexture* iconAmmo2;
	CHudTexture* iconCrosshair;
	CHudTexture* iconAutoaim;
	CHudTexture* iconZoomedCrosshair;
	CHudTexture* iconZoomedAutoaim;
	CHudTexture* iconSmall;
};

class CCSWeaponInfo : public FileWeaponInfo_t {
public:
	char* GetConsoleName() {
		return *( char** ) ( ( uintptr_t )this + 0x8);
	}

	int GetClipSize() {
		return *( int* ) ( ( uintptr_t )this + 0x20);
	}

	CSWeaponType GetWeaponType() {
		return *( CSWeaponType* ) ( ( uintptr_t )this + 0x140);
	}

	int GetDamage() {
		return *( int* ) ( ( uintptr_t )this + 0x16C);
	}

	float GetWeaponArmorRatio() {
		return *( float* ) ( ( uintptr_t )this + 0x170);
	}

	float GetPenetration() {
		return *( float* ) ( ( uintptr_t )this + 0x178);
	}

	float GetRange() {
		return *( float* ) ( ( uintptr_t )this + 0x184);
	}

	float GetRangeModifier() {
		return *( float* ) ( ( uintptr_t )this + 0x188);
	}

	float GetMaxPlayerSpeed() {
		return *( float* ) ( ( uintptr_t )this + 0x1B0);
	}

	int GetZoomLevels() {
		return *( int* ) ( ( uintptr_t )this + 0x23C);
	}

	char* GetTracerEffect() {
		return *( char** ) ( ( uintptr_t )this + 0x280);
	}

	int* GetTracerFrequency() {
		return ( int* ) ( ( uintptr_t )this + 0x288);
	}
};

class C_BaseCombatWeapon: public C_BaseAttributableItem
{
public:
	int GetOwner()
	{
		return *(int*)((uintptr_t)this + offsets.DT_BaseCombatWeapon.m_hOwner);
	}

	int GetAmmo()
	{
		return *(int*)((uintptr_t)this + offsets.DT_BaseCombatWeapon.m_iClip1);
	}

	float GetNextPrimaryAttack()
	{
		return *(float*)((uintptr_t)this + offsets.DT_BaseCombatWeapon.m_flNextPrimaryAttack);
	}

	bool GetInReload()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_BaseCombatWeapon.m_bInReload);
	}

	float GetAccuracyPenalty()
	{
		return *(float*)((uintptr_t)this + offsets.DT_WeaponCSBase.m_fAccuracyPenalty);
	}

    float GetPostPoneReadyTime()
    {
        return *(float*)((uintptr_t) this + offsets.DT_WeaponCSBase.m_flPostponeFireReadyTime);
    }

	bool GetReloadVisuallyComplete()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_WeaponCSBase.m_bReloadVisuallyComplete);
	}

	CCSWeaponInfo* GetCSWpnData() { // "script file not found"
		typedef CCSWeaponInfo* (* oGetCSWpnData)( void* );
		return getvfunc<oGetCSWpnData>( this, 522 )( this ); // 516
	}

	float GetSpread() {
		typedef float (* oGetSpread)( void* );
		return getvfunc<oGetSpread>( this, 514 )( this );
	}

	float GetInaccuracy() {
		typedef float (* oGetInaccuracy)( void* );
		return getvfunc<oGetInaccuracy>( this, 544 )( this );
	}

	void UpdateAccuracyPenalty() {
		typedef void (* oUpdateAccuracyPenalty)( void* );
		return getvfunc<oUpdateAccuracyPenalty>( this, 545 )( this );
	}

	char * GetIcon()
	{
		switch (*GetItemDefinitionIndex())
		{
		case ItemDefinitionIndex::WEAPON_KNIFE_BAYONET:
			return (char*)"1";
		case ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE:
			return (char*)"7";
		case ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY:
			return (char*)"8";
		case ItemDefinitionIndex::WEAPON_KNIFE:
			return (char*)"]";
		case ItemDefinitionIndex::WEAPON_KNIFE_FALCHION:
			return (char*)"0";
		case ItemDefinitionIndex::WEAPON_KNIFE_FLIP:
			return (char*)"2";
		case ItemDefinitionIndex::WEAPON_KNIFE_GUT:
			return (char*)"3";
		case ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT:
			return (char*)"4";
		case ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET:
			return (char*)"5";
		case ItemDefinitionIndex::WEAPON_KNIFE_T:
			return (char*)"[";
		case ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL:
			return (char*)"6";
		case ItemDefinitionIndex::WEAPON_KNIFE_PUSH:
			return (char*)"]";
		case ItemDefinitionIndex::WEAPON_DEAGLE:
			return (char*)"A";
		case ItemDefinitionIndex::WEAPON_ELITE:
			return (char*)"B";
		case ItemDefinitionIndex::WEAPON_FIVESEVEN:
			return (char*)"C";
		case ItemDefinitionIndex::WEAPON_GLOCK:
			return (char*)"D";
		case ItemDefinitionIndex::WEAPON_HKP2000:
			return (char*)"E";
		case ItemDefinitionIndex::WEAPON_P250:
			return (char*)"F";
		case ItemDefinitionIndex::WEAPON_USP_SILENCER:
			return (char*)"G";
		case ItemDefinitionIndex::WEAPON_TEC9:
			return (char*)"H";
		case ItemDefinitionIndex::WEAPON_REVOLVER:
			return (char*)"J";
		case ItemDefinitionIndex::WEAPON_MAC10:
			return (char*)"K";
		case ItemDefinitionIndex::WEAPON_UMP45:
			return (char*)"L";
		case ItemDefinitionIndex::WEAPON_BIZON:
			return (char*)"M";
		case ItemDefinitionIndex::WEAPON_MP7:
			return (char*)"N";
		case ItemDefinitionIndex::WEAPON_MP9:
			return (char*)"O";
		case ItemDefinitionIndex::WEAPON_P90:
			return (char*)"P";
		case ItemDefinitionIndex::WEAPON_GALILAR:
			return (char*)"Q";
		case ItemDefinitionIndex::WEAPON_FAMAS:
			return (char*)"R";
		case ItemDefinitionIndex::WEAPON_M4A1_SILENCER:
			return (char*)"S";
		case ItemDefinitionIndex::WEAPON_M4A1:
			return (char*)"T";
		case ItemDefinitionIndex::WEAPON_AUG:
			return (char*)"U";
		case ItemDefinitionIndex::WEAPON_SG556:
			return (char*)"V";
		case ItemDefinitionIndex::WEAPON_AK47:
			return (char*)"W";
		case ItemDefinitionIndex::WEAPON_G3SG1:
			return (char*)"X";
		case ItemDefinitionIndex::WEAPON_SCAR20:
			return (char*)"Y";
		case ItemDefinitionIndex::WEAPON_AWP:
			return (char*)"Z";
		case ItemDefinitionIndex::WEAPON_SSG08:
			return (char*)"a";
		case ItemDefinitionIndex::WEAPON_XM1014:
			return (char*)"b";
		case ItemDefinitionIndex::WEAPON_SAWEDOFF:
			return (char*)"c";
		case ItemDefinitionIndex::WEAPON_MAG7:
			return (char*)"d";
		case ItemDefinitionIndex::WEAPON_NOVA:
			return (char*)"e";
		case ItemDefinitionIndex::WEAPON_NEGEV:
			return (char*)"f";
		case ItemDefinitionIndex::WEAPON_M249:
			return (char*)"g";
		case ItemDefinitionIndex::WEAPON_TASER:
			return (char*)"h";
		case ItemDefinitionIndex::WEAPON_FLASHBANG:
			return (char*)"i";
		case ItemDefinitionIndex::WEAPON_HEGRENADE:
			return (char*)"j";
		case ItemDefinitionIndex::WEAPON_SMOKEGRENADE:
			return (char*)"k";
		case ItemDefinitionIndex::WEAPON_MOLOTOV:
			return (char*)"l";
		case ItemDefinitionIndex::WEAPON_DECOY:
			return (char*)"m";
		case ItemDefinitionIndex::WEAPON_INCGRENADE:
			return (char*)"n";
		case ItemDefinitionIndex::WEAPON_C4:
			return (char*)"o";
		case ItemDefinitionIndex::WEAPON_CZ75A:
			return (char*)"I";
		case ItemDefinitionIndex::WEAPON_KNIFE_URSUS:
			return (char*)"]";
		case ItemDefinitionIndex::WEAPON_KNIFE_GYPSY_JACKKNIFE:
			return (char*)"]";
		case ItemDefinitionIndex::WEAPON_KNIFE_STILETTO:
			return (char*)"]";
		case ItemDefinitionIndex::WEAPON_KNIFE_WIDOWMAKER:
			return (char*)"]";
		default:
			return (char*)"  ";
		}
	}
};

class C_WeaponC4 : C_BaseCombatWeapon
{
public:
	bool GetStartedArming()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_WeaponC4.m_bStartedArming);
	}
};

class C_Chicken : C_BaseEntity
{
public:
	bool* GetShouldGlow()
	{
		return (bool*)((uintptr_t)this + offsets.DT_DynamicProp.m_bShouldGlow);
	}
};

class C_BaseCSGrenade : C_BaseCombatWeapon
{
public:
	bool IsHeldByPlayer()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_BaseCSGrenade.m_bIsHeldByPlayer);
	}

	bool GetPinPulled()
	{
		return *(bool*)((uintptr_t)this + offsets.DT_BaseCSGrenade.m_bPinPulled);
	}

	float GetThrowTime()
	{
		return *(float*)((uintptr_t)this + offsets.DT_BaseCSGrenade.m_fThrowTime);
	}

	float GetThrowStrength()
	{
		return *(float*)((uintptr_t)this + offsets.DT_BaseCSGrenade.m_flThrowStrength);
	}
};
