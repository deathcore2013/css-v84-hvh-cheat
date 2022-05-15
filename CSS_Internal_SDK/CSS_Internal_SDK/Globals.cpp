#include "Globals.h"

HINSTANCE				g_hLibrary;
s_PlayerList_t          sPlayerList[ 64 ];

// Offsets
DWORD					cBoneMatrix			= 0x0;
DWORD					cGlobalVars			= 0x0;
DWORD					cWeaponId			= 0x0;
DWORD					cSpread				= 0x0;
DWORD					cCone				= 0x0;

DWORD					c_fFlags			= 0x0;
DWORD					c_lifeState			= 0x0;
DWORD					c_iTeamNum			= 0x0;
DWORD					c_iHealth			= 0x0;
DWORD					c_bIsDormant		= 0x0;
DWORD					c_vecOrigin			= 0x0;
DWORD					c_viewOrigin		= 0x0;
DWORD					c_iFOV				= 0x0;
DWORD					c_hActiveWeapon		= 0x0;
DWORD					c_szModelName		= 0x0;
DWORD					c_iClip1			= 0x0;
DWORD					c_vecPunch			= 0x0;
DWORD					c_flFlashDuration	= 0x0;
DWORD					c_bIsDefusing		= 0x0;

void InitOffsets()
{
	cWeaponId = 0x2C; //klar
	cCone = 0x6E0;
	cSpread = 0x3C; //klar?

	cBoneMatrix = 0x810;

	c_fFlags = 0x350; //klar
	c_lifeState = 0x93; //klar
	c_iTeamNum = 0x9C; //klar
	c_iHealth = 0x94; //klar
	c_bIsDormant = 0x17E; //fix?
	c_vecOrigin = 0x518; //fixad
	c_viewOrigin = 0xE8; //fixed
	c_iFOV = 0x0FD4; //fixed
	c_hActiveWeapon = 0x0D80; //fixed
	c_szModelName = 0x5C; //fixar sen
	c_iClip1 = 0x8BC; //Fixad
	c_vecPunch = 0x6C; //fixad
	c_flFlashDuration = 0x1454;//klar
	c_bIsDefusing = 0x140C;//klar

	cGlobalVars = 0x3C1A4C;
}

// Interfaces
IBaseClientDLL			*g_pClient		 = NULL;
IVEngineClient			*g_pEngine		 = NULL;
IEngineTrace			*g_pTrace		 = NULL;
IClientEntityList       *g_pEntList      = NULL;
CInput					*g_pInput		 = NULL;
IVModelInfo				*g_pModelInfo	 = NULL;
IVModelRender			*g_pModelRender	 = NULL;
IMaterialSystem			*g_pMatSystem	 = NULL;
pSurface				*g_pSurface      = NULL;
IPrediction             *g_pPred         = NULL;
IGameMovement           *g_pGameMovement = NULL;
IPanel                  *g_pIPanel       = NULL;
ICvar                   *g_pCvar         = NULL;
IPhysicsSurfaceProps    *g_pPhysics      = NULL;
IGameEventManager2      *g_EventManager2 = NULL;
CGlobalVarsBase			*pGlobals;

CreateMove_t			pCreateMove;
GetUserCmd_t			pGetUserCmd;
FrameStageNotify_t      pFrameStageNotify; 
HudUpdate_t             pHudUpdate;
FindMaterial_t			pFindMaterial;
PaintTraverse_t         pPaintTraverse;
KeyEvent_t              pKeyEvent;

// Hooks
VMTHookManager			*g_pClientManager;
VMTHookManager			*g_pInputManager;
VMTHookManager          *g_FrameStageNotify;
VMTHookManager          *g_pHudUpdate;
VMTHookManager			*g_FindMaterialManager;
VMTHookManager          *g_DrawingManager;
VMTHookManager          *g_KeyEventManager;

// CSS
HWND					hWnd;
DWORD					dwClient;
DWORD					dwEngine;
bool                    bInGame = false;
int                     x, y;
int                     g_iAntiAimType = 1, g_iFakeLaggEnd = 2;
