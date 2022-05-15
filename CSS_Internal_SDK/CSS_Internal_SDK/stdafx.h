#ifndef STDAFX_H
#define STDAFX_H

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <MMSystem.h>
#include <string>
#include <string.h>
#include <dwmapi.h>

#pragma comment( lib, "winmm.lib" )

#include "VmtHook.h"
#include "Tools.h"

#ifndef CLIENT_DLL
#define CLIENT_DLL
// First
#include "SDK\\public\\tier0\\wchartypes.h"

// Public
#include "SDK\\public\\cdll_int.h"
//#include "SDK\\public\\iprediction.h"
#include "SDK\\public\\bone_setup.h"
#include "SDK\\public\\icliententitylist.h"
#include "SDK\\public\\ienginevgui.h"
#include "SDK\\public\\IGameUIFuncs.h"
#include "SDK\\public\\dlight.h"
#include "SDK\\public\\iefx.h" 
#include "SDK\\public\\igameevents.h"
#include "SDK\\public\\view_shared.h"
#include "SDK\\public\\inetchannelinfo.h"
#include "SDK\\public\\iachievementmgr.h"
#include "SDK\\public\\steam\\steam_api.h"
#include "SDK\\public\\steam\\isteamuserstats.h"

// Client
#include "SDK\\game\\client\\imessagechars.h"
#include "SDK\\game\\client\\iclientmode.h"
#include "SDK\\game\\client\\cliententitylist.h"
#include "SDK\\game\\client\\cdll_client_int.h"
#include "SDK\\game\\client\\cbase.h"
#include "SDK\\game\\client\\c_baseanimating.h"
#include "SDK\\game\\client\\c_basecombatweapon.h"
#include "SDK\\game\\client\\c_baseplayer.h"
#include "SDK\\game\\client\\enginesprite.h"
#include "SDK\\game\\client\\input.h"
#include "SDK\\game\\client\\c_playerresource.h"
#include "SDK\\game\\client\\iviewrender.h"
#include "SDK\\game\\client\\viewrender.h"
#include "SDK\\game\\client\\game_controls\\commandmenu.h"
#include "SDK\\game\\client\\hudelement.h"
//#include "SDK\\game\\client\\prediction.h"


// Engine
#include "SDK\\public\\engine\\ivmodelrender.h"
#include "SDK\\public\\engine\\ivdebugoverlay.h"
#include "SDK\\public\\engine\\ivmodelinfo.h"
#include "SDK\\public\\engine\\IEngineTrace.h"
#include "SDK\\public\\engine\\IEngineSound.h"

// Material System
#include "SDK\\public\\materialsystem\\imaterialsystemstub.h"
#include "SDK\\public\\materialsystem\\itexture.h"
#include "SDK\\public\\materialsystem\\IMaterialVar.h"

// VGUI
#include "SDK\\public\\matsys_controls\\matsyscontrols.h"
#include "SDK\\public\\vgui\\IClientPanel.h"
#include "SDK\\public\\vgui\\IPanel.h"
#include "SDK\\public\\vgui\\ISurface.h"
#include "SDK\\public\\vgui\\ILocalize.h"
#include "SDK\\public\\collisionutils.h"

// VGUI Controls
#include "SDK\\public\\vgui_controls\\Panel.h"

// VGUI Material Surface
#include "SDK\\public\\VGuiMatSurface\\IMatSystemSurface.h"

// Shared
#include "SDK\\game\\shared\\usermessages.h"
#include "SDK\\game\\shared\\basecombatweapon_shared.h"
#include "SDK\\game\\shared\\takedamageinfo.h"
//#include "SDK\\game\shared\igamemovement.h"

// Others
#include "SDK\\game\\client\\c_vehicle_jeep.h"

#pragma comment(lib, "SDK/lib/public/tier0.lib")
#pragma comment(lib, "SDK/lib/public/tier1.lib")
#pragma comment(lib, "SDK/lib/public/tier2.lib")
#pragma comment(lib, "SDK/lib/public/tier3.lib")
#pragma comment(lib, "SDK/lib/public/mathlib.lib")
#pragma comment(lib, "SDK/lib/public/vstdlib.lib")
#pragma comment(lib, "SDK/lib/public/vtf.lib")
#pragma comment(lib, "SDK/lib/public/particles.lib")
#pragma comment(lib, "SDK/lib/public/matsys_controls.lib")
#pragma comment(lib, "SDK/lib/public/vgui_controls.lib")
#pragma comment(lib, "SDK/lib/public/bitmap.lib")
#pragma comment(lib, "SDK/lib/public/vmpi.lib")
#pragma comment(lib, "SDK/lib/public/raytrace.lib")
#pragma comment(lib, "SDK/lib/public/dmxloader.lib")
#pragma comment(lib, "SDK/lib/public/nvtristrip.lib")

#pragma warning( disable:4311 )
#pragma warning( disable:4312 )
#pragma warning( disable:4244 )
#pragma warning( disable:4541 )
#pragma warning( disable:4267 )
#pragma warning( disable:4183 )
#pragma warning( disable:4018 )
#pragma warning( default:4355 )
#pragma warning( default:4098 )
#pragma warning( disable:4409 )
#pragma warning( disable:4390 )
#pragma warning( disable:4172 )
#pragma warning( disable:4098 )

#endif

template< typename Function > Function CallVirtual( PVOID Base, DWORD Index )
{
	PDWORD* VTablePointer = ( PDWORD* )Base;
	PDWORD VTableFunctionBase = *VTablePointer;
	DWORD dwAddress = VTableFunctionBase[ Index ];

	return ( Function )( dwAddress );
}

#endif