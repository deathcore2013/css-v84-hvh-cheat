#include "DLLMain.h"

BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved )
{
	if( dwReason == DLL_PROCESS_ATTACH )
	{
		g_hLibrary = hModule;
		CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE ) HookThread, 0, 0, 0 );
		return true;
	}
}

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

CreateInterfaceFn CaptureFactory( char *FactoryModuleName )
{
	CreateInterfaceFn ret = NULL;

	while( !ret )
	{
		HMODULE FactoryModule = GetModuleHandleA( FactoryModuleName );

		if( FactoryModule ) ret = reinterpret_cast< CreateInterfaceFn >( GetProcAddress( FactoryModule, "CreateInterface" ) );
		Sleep( 10 );
	}
	return ret;
}

void *CaptureInterface( CreateInterfaceFn Fn, char *InterfaceName )
{
	unsigned long *ret = NULL;

	while( !ret )
	{
		ret = reinterpret_cast< unsigned long* >( Fn( InterfaceName, NULL ) );
		Sleep( 10 );
	}
	return ret;
}

void HookThread( )
{
	/*AllocConsole( );
	AttachConsole( GetCurrentProcessId( ) );
	freopen( "CON", "w", stdout );*/

	g_pLinker->UnlinkModuleFromPEB( g_hLibrary );

	while( !hWnd ) hWnd = FindWindow( NULL, "Counter-Strike Source" );
	while( dwClient  == 0x0 ) dwClient  = ( DWORD )GetModuleHandle( "client.dll" );
	while( dwEngine  == 0x0 ) dwEngine  = ( DWORD )GetModuleHandle( "engine.dll" );

	InitOffsets( );

	CreateInterfaceFn fnEngine		= CaptureFactory( "engine.dll" );
	CreateInterfaceFn fnClient		= CaptureFactory( "client.dll" );
	CreateInterfaceFn fnMaterial	= CaptureFactory( "materialsystem.dll" );
	CreateInterfaceFn fnVGUI        = CaptureFactory( "vguimatsurface.dll" );
	CreateInterfaceFn fnVGUI2       = CaptureFactory( "vgui2.dll" );
	CreateInterfaceFn fnCvar        = CaptureFactory( "vstdlib.dll" );
	CreateInterfaceFn fnVPhysics	= CaptureFactory( "vphysics.dll" );

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE );
	printf( "Searching Interfaces..." );

	g_pClient       = reinterpret_cast< IBaseClientDLL*>( CaptureInterface( fnClient, "VClient017" ) );
	PDWORD dwClient = NULL;
	memcpy( &dwClient, ( void * )( g_pClient ), 4 );
	g_pInput        = ( CInput* )*( PDWORD )*( PDWORD )( dwClient[ 21 ] + 0x20 );

	g_pEntList   	= reinterpret_cast< IClientEntityList *> ( CaptureInterface( fnClient, "VClientEntityList003" ) );
	g_pEngine		= reinterpret_cast< IVEngineClient* >( CaptureInterface( fnEngine, "VEngineClient014" ) ); //обновлен
	g_pTrace		= reinterpret_cast< IEngineTrace* > ( CaptureInterface( fnEngine, "EngineTraceClient003" ) );
	g_pModelInfo	= reinterpret_cast< IVModelInfo* > ( CaptureInterface( fnEngine, VMODELINFO_CLIENT_INTERFACE_VERSION ) );
	g_pModelRender	= reinterpret_cast< IVModelRender *>( CaptureInterface( fnEngine, VENGINE_HUDMODEL_INTERFACE_VERSION ) );
	g_pMatSystem	= reinterpret_cast< IMaterialSystem *>( CaptureInterface( fnMaterial, "VMaterialSystem080" ) );
	g_pSurface		= reinterpret_cast< pSurface *>( CaptureInterface( fnVGUI, VGUI_SURFACE_INTERFACE_VERSION ) );
	g_pGameMovement = reinterpret_cast< IGameMovement *>( CaptureInterface( fnClient, "GameMovement001" ) ); 
	g_pPred         = reinterpret_cast< IPrediction *>( CaptureInterface( fnClient, "VClientPrediction001" ) );
	g_pIPanel       = reinterpret_cast< IPanel *> ( CaptureInterface( fnVGUI2, "VGUI_Panel009" ) );
	g_pCvar         = reinterpret_cast< ICvar * >( CaptureInterface( fnCvar, "VEngineCvar004" ) );
	g_pPhysics		= reinterpret_cast< IPhysicsSurfaceProps *>( CaptureInterface( fnVPhysics, "VPhysicsSurfaceProps001") );
	g_EventManager2 = reinterpret_cast< IGameEventManager2 *>( CaptureInterface( fnEngine, "GAMEEVENTSMANAGER002" ) );

	if( g_pEntList && g_pEngine && g_pTrace && g_pModelInfo && g_pModelRender && g_pMatSystem && g_pSurface && g_pPred && g_pGameMovement && g_pCvar && g_pIPanel && g_pPhysics && g_pInput && g_EventManager2 )
	{
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN );
		printf( "Ok!\n" );
	}
	else
	{
		SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED );
		printf( "Failed!\n" );
		return;
	}

	g_pClientManager = new VMTHookManager( ( PDWORD* )g_pClient );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE );
	printf( "Hooking CreateMove..." );
	pCreateMove = ( CreateMove_t )g_pClientManager->dwHookMethod( ( DWORD ) hkCreateMove, 21 );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN );
	printf( "Ok!\n" );

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE );
	g_pInputManager = new VMTHookManager( (PDWORD* )g_pInput );
    printf( "Hooking GetUserCmd..." );
	pGetUserCmd = ( GetUserCmd_t )g_pInputManager->dwHookMethod( ( DWORD ) hkGetUserCmd, 8 );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN );
	printf( "Ok!\n" );

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE );
	g_FrameStageNotify = new VMTHookManager( ( PDWORD* )g_pClient );
	printf( "Hooking FrameStageNotify..." );
	pFrameStageNotify = ( FrameStageNotify_t )g_FrameStageNotify->dwHookMethod( ( DWORD ) hkFrameStageNotify, 35 );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN );
	printf( "Ok!\n" );

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE );
	g_pHudUpdate = new VMTHookManager( ( PDWORD* )g_pClient );
	printf( "Hooking FrameStageNotify..." );
	pHudUpdate = ( HudUpdate_t )g_pHudUpdate->dwHookMethod( ( DWORD ) hkHudUpdate, 11 );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN );
	printf( "Ok!\n" );

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE );
	g_FindMaterialManager = new VMTHookManager( ( PDWORD* )g_pMatSystem);
	printf( "Hooking FindMaterial..." );
	pFindMaterial = ( FindMaterial_t )g_FindMaterialManager->dwHookMethod( ( DWORD ) hkFindMaterial, 71 );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN );
	printf( "Ok!\n" );

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE );
	g_DrawingManager = new VMTHookManager( ( PDWORD* )g_pIPanel );
	printf( "Hooking PaintTraverse..." );
	pPaintTraverse = ( PaintTraverse_t )g_DrawingManager->dwHookMethod( ( DWORD ) hkPaintTraverse, 41 );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN );
	g_pDraw->Initialize( );
	printf( "Ok!\n" );

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE );
	g_KeyEventManager = new VMTHookManager( ( PDWORD* )g_pClient );
	printf( "Hooking KeyEvent..." );
	pKeyEvent = ( KeyEvent_t )g_KeyEventManager->dwHookMethod( ( DWORD ) hkKeyEvent, 20 );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN );
	g_pConsole->Init( );
	printf( "Ok!\n" );

	gEvent.RegisterSelf( );
	gEvent.Register( );
}
