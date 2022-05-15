#ifndef GLOBALS_H
#define GLOBALS_H

#include "stdafx.h"
#include "Prediction.h"
#include "Surface.h"

extern void InitOffsets( );

extern CGlobalVarsBase *pGlobals;

struct s_PlayerList_t
{
	bool  bIsFriend;
	bool  bMagicPitch;
	bool  bBodyAim;
	float flLostTick;
	float old_flTickCount;
} extern sPlayerList[ 64 ];

struct WeaponInfo_s
{
	int iWeaponID;
	int iPenetration;
	int fCurrentDamage;
	float flDistance;
	float flRangeModifier;
	int iBulletsPerShot;
	int iAmmoType;
	float flPenetrationDistance;
	float flPenetrationPower;
}; 

enum Weapontypes_t
{
	WEAPON_PISTOL = 1,
	WEAPON_SHOTGUN,
	WEAPON_MP,
	WEAPON_RIFLE,
	WEAPON_SNIPER,
	WEAPON_MG,
	WEAPON_OTHER,
};

enum Weapons_t 
{
	WEAPON_P228 = 1,
	WEAPON_GLOCK,
	WEAPON_SCOUT,
	WEAPON_HE,
	WEAPON_XM1014,
	WEAPON_C4,
	WEAPON_MAC10,
	WEAPON_AUG,
	WEAPON_SMOKE,
	WEAPON_ELITE, 
	WEAPON_FIVE7,
	WEAPON_UMP45, 
	WEAPON_SG550,
	WEAPON_GALIL, 
	WEAPON_FAMAS, 
	WEAPON_USP45, 
	WEAPON_AWP, 
	WEAPON_MP5, 
	WEAPON_M249, 
	WEAPON_M3SUPER,
	WEAPON_M4A1, 
	WEAPON_TMP,
	WEAPON_G3SG1,
	WEAPON_FLASH,
	WEAPON_DEAGLE,
	WEAPON_SG552, 
	WEAPON_AK47, 
	WEAPON_KNIFE,
	WEAPON_P90,
};

extern HINSTANCE			g_hLibrary;

#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)
#define IN_SPEED					(1 << 17)
#define IN_WALK						(1 << 18)
#define IN_ZOOM						(1 << 19)
#define IN_WEAPON1					(1 << 20)
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)

// Offsets
extern DWORD				cBoneMatrix;
extern DWORD				cGlobalVars;
extern DWORD				cWeaponId;
extern DWORD				cSpread;
extern DWORD				cCone;

extern DWORD				c_fFlags;
extern DWORD				c_lifeState;
extern DWORD				c_iTeamNum;
extern DWORD				c_iHealth;
extern DWORD				c_bIsDormant;
extern DWORD				c_vecOrigin;
extern DWORD				c_viewOrigin;
extern DWORD				c_iFOV;
extern DWORD				c_hActiveWeapon;
extern DWORD				c_szModelName;
extern DWORD				c_iClip1;
extern DWORD				c_vecPunch;
extern DWORD				c_flFlashDuration;
extern DWORD				c_bIsDefusing;

class IPrediction;
class IGameMovement;
class pSurface;

// Interfaces
extern IBaseClientDLL		*g_pClient;
extern IVEngineClient		*g_pEngine;
extern IEngineTrace			*g_pTrace;
extern IClientEntityList    *g_pEntList;
extern CInput				*g_pInput;
extern IVModelInfo		    *g_pModelInfo;
extern IVModelRender		*g_pModelRender;
extern IMaterialSystem		*g_pMatSystem;
extern pSurface				*g_pSurface;
extern IPrediction          *g_pPred;
extern IGameMovement        *g_pGameMovement;
extern IPanel               *g_pIPanel;
extern ICvar                *g_pCvar;
extern IPhysicsSurfaceProps *g_pPhysics;
extern IGameEventManager2   *g_EventManager2;

// Hooks
extern VMTHookManager		*g_pClientManager;
extern VMTHookManager		*g_pInputManager;
extern VMTHookManager       *g_FrameStageNotify;
extern VMTHookManager       *g_pHudUpdate;
extern VMTHookManager		*g_FindMaterialManager;
extern VMTHookManager       *g_DrawingManager;
extern VMTHookManager       *g_KeyEventManager;

// CSS
extern HWND					hWnd;
extern DWORD				dwClient;
extern DWORD				dwEngine;
extern bool                 bInGame;
extern int                  x, y;
extern int                  g_iAntiAimType, g_iFakeLaggEnd;

typedef CUserCmd*( __stdcall *GetUserCmd_t)( int sequence_number );
typedef void( __stdcall *CreateMove_t) ( int sequence_number, float input_sample_frametime, bool active );;
extern GetUserCmd_t pGetUserCmd;

extern void __stdcall hkCreateMove( int sequence_number, float input_sample_frametime, bool active );
extern CUserCmd* __stdcall hkGetUserCmd( int sequence_number );
extern CreateMove_t pCreateMove;

typedef void( __stdcall *FrameStageNotify_t) ( ClientFrameStage_t curStage );
extern void __stdcall hkFrameStageNotify( ClientFrameStage_t curStage );
extern FrameStageNotify_t pFrameStageNotify;

typedef void( __stdcall *HudUpdate_t) ( bool bActive );
extern void __stdcall hkHudUpdate( bool bActive );
extern HudUpdate_t pHudUpdate;

typedef IMaterial*(__stdcall *FindMaterial_t)( char const* pMaterialName, const char *pTextureGroupName, bool complain, const char* pComplainPrefix );
extern IMaterial* __stdcall hkFindMaterial( char const* pMaterialName, const char* pTextureGroupName, bool complain, const char* pComplainPrefix );
extern FindMaterial_t pFindMaterial;

typedef void( __stdcall *PaintTraverse_t) ( unsigned int vguiPanel, bool forceRepaint, bool allowForce );
extern void __stdcall hkPaintTraverse( unsigned int vguiPanel, bool forceRepaint, bool allowForce );
extern PaintTraverse_t pPaintTraverse;

typedef int( __stdcall *KeyEvent_t) ( int eventcode, int keynum, const char *pszCurrentBinding );
extern int __stdcall hkKeyEvent( int eventcode, int keynum, const char *pszCurrentBinding );
extern KeyEvent_t pKeyEvent;

#endif