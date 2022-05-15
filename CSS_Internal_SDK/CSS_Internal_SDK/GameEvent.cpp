#include "cGameEvent.h"

#define EVENT_HOOK( x )

cGameEvent gEvent;

void cGameEvent::FireGameEvent( IGameEvent *event )
{
	const char* szEventName = event->GetName( );
	if( !szEventName )	return;

	if( strcmp( szEventName, "player_connect" ) == 0 )
	{
		string one = "Player ";
		one += event->GetString( "name" );
		one += " connected, IP : ";
		one += event->GetString( "address" );
		one += ", SteamID : ";
		one += event->GetString( "networkid" );
		g_pConsole->echo( one.c_str( ) );
		g_pConsole->scrollCount = 0;
		g_pConsole->lineIndex = g_pConsole->prevLines.size( ) + 1;
	}	

	/*if( strcmp( szEventName, "round_start" ) == 0 && g_bAutoWeapon )
	{
		g_pEngine->ClientCmd( czWeapon );
	}*/

	if( strcmp( szEventName, "player_hurt" ) == 0 )
	{
		player_info_t pInfo;
		if( !g_pEngine->GetPlayerInfo( g_pEngine->GetLocalPlayer( ), &pInfo ) ) return;
		if( pInfo.userID == event->GetInt( "attacker" ) )
		{
			DamageText[ m_iDamageText ] = DamageText_t( event->GetInt( "dmg_health" ), ( char* )event->GetString( "weapon" ), clock( ) + 3000.f );
			m_iDamageText++;
			if( m_iDamageText == 15 ) m_iDamageText = 0;
		}
	}
}

void cGameEvent::RegisterSelf( )
{
	sprintf( czWeapon, "say !m249 !helmet" );

	m_iDamageText = 0;

	//g_EventManager2->AddListener( this, "round_start", false );
	g_EventManager2->AddListener( this, "player_connect", false );
	g_EventManager2->AddListener( this, "player_hurt", false );
}

void cGameEvent::Register( )
{
	EVENT_HOOK( FireEvent );
}