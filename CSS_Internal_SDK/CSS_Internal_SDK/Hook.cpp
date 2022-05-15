#include "Hook.h"

static int iAntiAimStart = 0;
static int iAintiAim = 1;
static bool bFlip = true;
static int iFlip = 0;

CUserCmd* __stdcall hkGetUserCmd( int sequence_number )
{
	DWORD GetUserCmd = *( DWORD* )( ( DWORD )g_pInput + 0xC4 ) + ( sequence_number % 90 << 6 );

	if( !GetUserCmd ) 
	{ 
		return pGetUserCmd( sequence_number );
	}

	CUserCmd *pCmd = ( CUserCmd* )GetUserCmd;

	pCmd->random_seed = 165;
	pCmd->command_number = 2087;

	return pCmd;
}

void __stdcall hkCreateMove( int sequence_number, float input_sample_frametime, bool active )
{
	pCreateMove( sequence_number, input_sample_frametime, active );

	if( !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) || g_pEngine->Con_IsVisible( ) || g_pEngine->IsDrawingLoadingImage( ) || !g_pInput ) return;

	CInput::CVerifiedUserCmd *pVerifiedCommands = *( CInput::CVerifiedUserCmd** )( ( DWORD )g_pInput + 0xC8 );
	if( !pVerifiedCommands ) return;

	CInput::CVerifiedUserCmd *pVerified = &pVerifiedCommands[ sequence_number % 90 ];

	CUserCmd* pCmd = hkGetUserCmd( sequence_number );
	if( !pCmd || !pVerified ) return; 

	cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if( !pLocal ) return;

	pGlobals = ( CGlobalVarsBase* )( dwEngine + cGlobalVars );

	DWORD_PTR dwebp;
	__asm  { mov dwebp , ebp }
	bool *bSendPackets = ( bool *)( *( PDWORD )( dwebp ) - 0x1 );

	bool bKeysPressed = true;
	if( GetAsyncKeyState( 0x41 ) || GetAsyncKeyState( 0x57 ) || GetAsyncKeyState( 0x53 ) || GetAsyncKeyState( 0x44 ) ) bKeysPressed = false;

	static bool bDirection = false;

	if( ( GetAsyncKeyState( VK_SPACE ) && ( pLocal->GetFlags( ) & FL_ONGROUND ) ) && bKeysPressed )
	{
		if( pLocal->GetVelocity( ).Length( ) <= 50 ) 
		{
			pCmd->forwardmove = 800.f;
		}
	}

	if( !( pLocal->GetFlags( ) & FL_ONGROUND ) && pCmd->buttons & IN_JUMP )
	{
		if( pLocal->GetVelocity( ).Length( ) <= 50 ) 
		{
			pCmd->forwardmove = 800.f;
		}
		pCmd->buttons &= ~IN_JUMP;
	}

	float flYawBhop = 0.f;
	if( pLocal->GetVelocity( ).Length( ) > 50.f ) 
	{
		float x = 30.f, y = pLocal->GetVelocity( ).Length( ), z = 0.f, a = 0.f;

		z = x / y;
		z = fabsf( z );

		a = x * z;

		flYawBhop = a;
	}

	if( ( GetAsyncKeyState( VK_SPACE ) && !( pLocal->GetFlags( ) & FL_ONGROUND ) ) && bKeysPressed )
	{
		if( pCmd->mousedx < 0 )
		{
			pCmd->sidemove = -400.f;
		}

		if( pCmd->mousedx > 0 )
		{
			pCmd->sidemove = 400.f;
		}

		if( bDirection ) 
		{
			pCmd->viewangles.y -= flYawBhop;
			pCmd->sidemove = -400.f;
			bDirection = false;
		}
		else
		{
			pCmd->viewangles.y += flYawBhop;
			pCmd->sidemove = 400.f;
			bDirection = true;
		}
	}

	Vector vMove( pCmd->forwardmove, pCmd->sidemove, pCmd->upmove );
	float flSpeed = sqrt( vMove.x * vMove.x + vMove.y * vMove.y ), flYaw;
	QAngle qMove, qRealView( pCmd->viewangles );
	VectorAngles( vMove, qMove );

	QAngle vRecoil = pLocal->GetPunchAngles( );
	vRecoil *= 1.98f;
	vRecoil = DecayPunchangle( vRecoil );

	//pPred( pCmd, ( C_BasePlayer* )pLocal ); 
	//FakeLaggPred( );

	g_pCvar->FindVar( "cl_interpolate" )->SetValue( "1" ); 
	g_pCvar->FindVar( "net_graph" )->SetValue( "0" );
	g_pCvar->FindVar( "r_drawparticles" )->SetValue( "0" );

	bool bAimbot = false;
	bool bAntiAim = false;

	if( pEngine->GetWeaponType( pLocal->GetWeaponID( ) ) > 0 )
	{
		bAntiAim = true;
		if( pLocal->CanShoot( ) )
		{
			bAimbot = pAimbot->AimbotMain( pCmd );
			if( bAimbot )
			{
				pCmd->viewangles -= vRecoil;
				pCmd->viewangles.NormalizeAngles( );

				flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
				pCmd->forwardmove = cos( flYaw ) * flSpeed;
				pCmd->sidemove = sin( flYaw ) * flSpeed;

				pCmd->buttons |= IN_ATTACK;
				bSendPackets = false;
				bAntiAim = false;
				iAntiAimStart = 0;
				bFlip = true;
				iFlip = 0;
			}
			if( !bAimbot && pCmd->buttons & IN_ATTACK )
			{
				static QAngle vSpread = QAngle( 0.0f, 0.0f, 0.0f );
				pNoSpread->GetSpreadFix( pCmd->random_seed, pCmd->viewangles, vSpread );
				pCmd->viewangles = pCmd->viewangles + vSpread - vRecoil;
				pCmd->viewangles.NormalizeAngles( );

				flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
				pCmd->forwardmove = cos( flYaw ) * flSpeed;
				pCmd->sidemove = sin( flYaw ) * flSpeed;

				bSendPackets = false;
				bAntiAim = false;
				iAntiAimStart = 0;
				bFlip = true;
				iFlip = 0;
			}
		}
	}

	bool bEdgeAA = false;
	bool bCrouchAA = false;

	if( GetAsyncKeyState( VK_CONTROL ) && bAntiAim )
	{
		static bool bDuck = false;
		if( bDuck )
		{
			bDuck = false;
			pCmd->buttons &= ~IN_DUCK;
			*bSendPackets = false;
			bEdgeAA = EdgeAntiAim( pLocal, pCmd, 180.f );
			if( bEdgeAA )
			{
				pCmd->viewangles.x = 89.999f;

				flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
				pCmd->forwardmove = cos( flYaw ) * flSpeed;
				pCmd->sidemove = sin( flYaw ) * flSpeed;
			}
			else
			{
				pCmd->viewangles.x = 180;
				pCmd->viewangles.y = GetAntiAimAngles( );

				flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
				pCmd->forwardmove = -cos( flYaw ) * flSpeed;
				pCmd->sidemove = sin( flYaw ) * flSpeed;
			}
		}
		else
		{
			if( pCmd->buttons & IN_DUCK )
			{
				*bSendPackets = true;
				bDuck = true;
				static bool bFakeCrouch = true;
				bFakeCrouch = !bFakeCrouch;
				bEdgeAA = EdgeAntiAim( pLocal, pCmd, 360.f );
				if( bEdgeAA )
				{
					pCmd->viewangles.x = 89.999f;

					flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
					pCmd->forwardmove = cos( flYaw ) * flSpeed;
					pCmd->sidemove = sin( flYaw ) * flSpeed;
				}
				else
				{
					pCmd->viewangles.x = -89.999;
					pCmd->viewangles.y = GetAntiAimAngles( ) + 90;

					flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
					pCmd->forwardmove = cos( flYaw ) * flSpeed;
					pCmd->sidemove = sin( flYaw ) * flSpeed;
				}
			}
		}
		bCrouchAA = true;
	}

	if( bAntiAim && !bCrouchAA )
	{
		if( m_MoveType( pLocal ) == MOVETYPE_LADDER )
		{
			flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
			pCmd->forwardmove = cos( flYaw ) * flSpeed;
			pCmd->sidemove = sin( flYaw ) * flSpeed;
		}
		else
		{
			bEdgeAA = EdgeAntiAim( pLocal, pCmd, 360.f );
			if( bEdgeAA )
			{
				*bSendPackets = true;
				pCmd->viewangles.x = 90;
				flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
				pCmd->forwardmove = -cos( flYaw ) * flSpeed;
				pCmd->sidemove = sin( flYaw ) * flSpeed;
			}
		}
	}

	if( bAntiAim && !bEdgeAA && g_iAntiAimType == 1 && !bCrouchAA )
	{
		if( m_MoveType( pLocal ) == MOVETYPE_LADDER )
		{
			*bSendPackets = true;
			flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
			pCmd->forwardmove = cos( flYaw ) * flSpeed;
			pCmd->sidemove = sin( flYaw ) * flSpeed;
		}
		else
		{
			iAntiAimStart++;

			if( iAintiAim == 1 )
			{
				if( iAntiAimStart <= 1 )
				{
					*bSendPackets = true;

					pCmd->viewangles.x = 180; 
					pCmd->viewangles.y = GetAntiAimAngles2( ) - 155;

					flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
					pCmd->forwardmove = -cos( flYaw ) * flSpeed;
					pCmd->sidemove = sin( flYaw ) * flSpeed;

				}
				if( iAntiAimStart > 1 && iAntiAimStart <= g_iFakeLaggEnd )
				{
					*bSendPackets = false;

					pCmd->viewangles.x = -50.087936; 
					pCmd->viewangles.y = GetAntiAimAngles2( ) + 115.087936;

					flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
					pCmd->forwardmove = cos( flYaw ) * flSpeed;
					pCmd->sidemove = sin( flYaw ) * flSpeed;
				}
			}

			if( iAintiAim == 2 )
			{
				if( iAntiAimStart <= 1 )
				{
					*bSendPackets = true;

					pCmd->viewangles.x = 180; 
					pCmd->viewangles.y = GetAntiAimAngles2( ) - 155;

					flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
					pCmd->forwardmove = -cos( flYaw ) * flSpeed;
					pCmd->sidemove = sin( flYaw ) * flSpeed;

				}
				if( iAntiAimStart > 1 && iAntiAimStart <= g_iFakeLaggEnd )
				{
					*bSendPackets = false;

					pCmd->viewangles.x = -50.087936; 
					pCmd->viewangles.y = GetAntiAimAngles2( ) + 58.087936;

					flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
					pCmd->forwardmove = cos( flYaw ) * flSpeed;
					pCmd->sidemove = sin( flYaw ) * flSpeed;
				}
			}
		}
	}

	if( bAntiAim && !bEdgeAA && g_iAntiAimType == 2 && !bCrouchAA )
	{
		if( m_MoveType( pLocal ) == MOVETYPE_LADDER )
		{
			*bSendPackets = true;
			flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
			pCmd->forwardmove = cos( flYaw ) * flSpeed;
			pCmd->sidemove = sin( flYaw ) * flSpeed;
		}
		else
		{
			static int iLagSwitch = 0;
			static float flAngles = 0.f;

			iLagSwitch = g_iFakeLaggEnd;
			flAngles = GetAntiAimAngles( );

			if( bFlip )
			{
				*bSendPackets = true;
				pCmd->viewangles.x = 180.087936; 
				pCmd->viewangles.y = flAngles + 89.999f;

				flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
				pCmd->forwardmove = -cos( flYaw ) * flSpeed;
				pCmd->sidemove = sin( flYaw ) * flSpeed;

				bFlip = false; 
			}
			else
			{
				*bSendPackets = false;
				pCmd->viewangles.x = 180.087936; 
				pCmd->viewangles.y = flAngles - 89.999f;

				flYaw = DEG2RAD( pCmd->viewangles.y - qRealView.y + qMove.y );
				pCmd->forwardmove = -cos( flYaw ) * flSpeed;
				pCmd->sidemove = sin( flYaw ) * flSpeed;

				if( iFlip < iLagSwitch ) iFlip++;
				else
				{
					bFlip = true;
					iFlip = 0;
				}
			}
		}
	}

	if( iAntiAimStart == g_iFakeLaggEnd || iAntiAimStart > g_iFakeLaggEnd )
	{
		iAntiAimStart = 0;
	}

	pVerified->m_cmd = *pCmd;
	pVerified->m_crc =  pCmd->GetChecksum( );
}

void __stdcall hkFrameStageNotify( ClientFrameStage_t curStage )
{
	Vector *PunchAngle, OldPunch;

	if( curStage == FRAME_RENDER_START )
	{
		if( g_pEngine->IsInGame( ) )
		{
			cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
			if( pLocal && pLocal->IsAlive( ) )
			{
				PunchAngle = ( Vector* )( ( DWORD )pLocal + c_vecPunch );
				OldPunch = *PunchAngle;
				*PunchAngle = Vector( 0.0f, 0.0f, 0.0f );
			}
		}
	}

	pFrameStageNotify( curStage );

	if( curStage == FRAME_RENDER_START )
	{
		if( g_pEngine->IsInGame( ) )
		{
			cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
			if( pLocal && pLocal->IsAlive( ) )
			{
				*PunchAngle = OldPunch;
			}
		}
	}
}

void __stdcall hkHudUpdate( bool bActive )
{
	pHudUpdate( bActive );

	static bool bOnce = false;
	if( !bOnce )
	{
		bOnce = !bOnce;
		ClientClass *pClass = g_pClient->GetAllClasses( );

		while( pClass )
		{
			const char *pszName = pClass->m_pRecvTable->GetName( );

			if( !strcmp( pszName, "DT_CSPlayer" ) )
			{
				for( int i = 0; i < pClass->m_pRecvTable->m_nProps; i++ )
				{
					RecvProp *pProp = pClass->m_pRecvTable->GetProp( i );

					if( !strcmp( pProp->GetName( ), "m_angEyeAngles[0]" ) )
						pProp->SetProxyFn( FixXRegular );
					if( !strcmp( pProp->GetName( ), "m_angEyeAngles[1]" ) )
						pProp->SetProxyFn( FixYRegular );
				}
			}
			pClass = pClass->m_pNext;
		}
	}
}

IMaterial* __stdcall hkFindMaterial( char const* pMaterialName, const char* pTextureGroupName, bool complain, const char* pComplainPrefix )
{
	IMaterial* pTemp = NULL;
	pTemp = pFindMaterial( pMaterialName, pTextureGroupName, complain, pComplainPrefix );

	if( pMaterialName && pTextureGroupName )
	{
		if( g_pEngine->IsInGame( ) )
		{
			if( !bInGame )
			{
				SetAsusWalls( 0.6f );
				SetSkyChams( false );
				SetPlayerChams( false, false );
				SetWeaponChams( false, false );
				bInGame = true;
			}
		}
		else
		{
			if( bInGame )
			{
				vecSkyTextures.clear( );//Clear Buffer
				SetSkyChams( false );//Fixing Server Error
				SetPlayerChams( false, false );//Fixing Server Error
				SetWeaponChams( false, false );//Fixing Server Error
				vecMapTextures.clear( );//Clear Buffer
				bInGame = false;
			}
			if( strstr( pTextureGroupName, "World" ) && !strstr( pMaterialName, "models\\player" ) )//World Materials
				vecMapTextures.push_back( pTemp );
			if( strstr( pTextureGroupName, "SkyBox textures" ) )//Skybox
				vecSkyTextures.push_back( pTemp );
			if( strstr( pTextureGroupName, "Model texture" ) && strstr( pMaterialName, "models\\player" ) )//Player Materials
				vecPlayerTextures.push_back( pTemp );
			if( strstr( pTextureGroupName, "Model textures" ) && strstr( pMaterialName, "models\\weapons" ) )
				vecWeaponTextures.push_back( pTemp );
		}
	}
	return pTemp;
}

void __stdcall hkPaintTraverse( unsigned int vguiPanel, bool forceRepaint, bool allowForce )
{
	pPaintTraverse( vguiPanel, forceRepaint, allowForce );

	const char* pszPanelName = g_pIPanel->GetName( vguiPanel );
	if( strstr( pszPanelName, "FocusOverlayPanel" ) )
	{
		if( !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) || g_pEngine->Con_IsVisible( ) || g_pEngine->IsDrawingLoadingImage( ) ) return;

		pGlobals = ( CGlobalVarsBase* )( dwEngine + cGlobalVars );

		for( int iHitMarker = 0; iHitMarker < 15; iHitMarker++ )
		{
			DamageText_t m_DText = gEvent.GetDamageText( iHitMarker ); 
			if( m_DText.flEnd >= clock( ) )
			{
				g_pDraw->DrawString( true, x / 2, y / 5 + ( int )( ( m_DText.flEnd - clock( ) ) * 0.1 ), Color( 255, 75, 25, 255 * ( int )( m_DText.flEnd - clock( ) ) / 3000 ), "%d", m_DText.iDamage );
			}
		}

		pESP.DrawPlayer( );
		g_pDraw->PlayerList( 40, 300, 100, 0 );
		g_pDraw->CrossHair( 10 );
		g_pConsole->Draw( );

		if( !g_pEngine->GetPlayerInfo( g_pEngine->GetLocalPlayer( ), &pESP.Info ) ) return;
		std::string szText = " hat einen extrem dicken Penis", szName;
		szName = pESP.Info.name + szText;
		g_pDraw->DrawString( true, 5, 5, Color( 255, 255, 255, 255 ), "%s", szName );
	}
}

int __stdcall hkKeyEvent( int eventcode, int keynum, const char *pszCurrentBinding )
{
	if( eventcode == 1 )
	{
		if( keynum == KEY_DELETE )
		{
			g_pConsole->active = !g_pConsole->active;
			return 0;
		}

		if( g_pConsole->active )
		{
			if( keynum == KEY_A )
			{
				g_pConsole->cmdLine += "a";
				return 0;
			}

			if( keynum == KEY_B )
			{
				g_pConsole->cmdLine += "b";
				return 0;
			}

			if( keynum == KEY_C )
			{
				g_pConsole->cmdLine += "c";
				return 0;
			}

			if( keynum == KEY_D )
			{
				g_pConsole->cmdLine += "d";
				return 0;
			}

			if( keynum == KEY_E )
			{
				g_pConsole->cmdLine += "e";
				return 0;
			}

			if( keynum == KEY_F )
			{
				g_pConsole->cmdLine += "f";
				return 0;
			}

			if( keynum == KEY_G )
			{
				g_pConsole->cmdLine += "g";
				return 0;
			}

			if( keynum == KEY_H )
			{
				g_pConsole->cmdLine += "h";
				return 0;
			}

			if( keynum == KEY_I )
			{
				g_pConsole->cmdLine += "i";
				return 0;
			}

			if( keynum == KEY_J )
			{
				g_pConsole->cmdLine += "j";
				return 0;
			}

			if( keynum == KEY_K )
			{
				g_pConsole->cmdLine += "k";
				return 0;
			}

			if( keynum == KEY_L )
			{
				g_pConsole->cmdLine += "l";
				return 0;
			}

			if( keynum == KEY_M )
			{
				g_pConsole->cmdLine += "m";
				return 0;
			}

			if( keynum == KEY_N )
			{
				g_pConsole->cmdLine += "n";
				return 0;
			}

			if( keynum == KEY_O )
			{
				g_pConsole->cmdLine += "o";
				return 0;
			}

			if( keynum == KEY_P )
			{
				g_pConsole->cmdLine += "p";
				return 0;
			}

			if( keynum == KEY_Q )
			{
				g_pConsole->cmdLine += "q";
				return 0;
			}

			if( keynum == KEY_R )
			{
				g_pConsole->cmdLine += "r";
				return 0;
			}

			if( keynum == KEY_S )
			{
				g_pConsole->cmdLine += "s";
				return 0;
			}

			if( keynum == KEY_T )
			{
				g_pConsole->cmdLine += "t";
				return 0;
			}

			if( keynum == KEY_U )
			{
				g_pConsole->cmdLine += "u";
				return 0;
			}

			if( keynum == KEY_V )
			{
				g_pConsole->cmdLine += "v";
				return 0;
			}

			if( keynum == KEY_W )
			{
				g_pConsole->cmdLine += "w";
				return 0;
			}

			if( keynum == KEY_X )
			{
				g_pConsole->cmdLine += "x";
				return 0;
			}

			if( keynum == KEY_Y )
			{
				g_pConsole->cmdLine += "y";
				return 0;
			}

			if( keynum == KEY_Z )
			{
				g_pConsole->cmdLine += "z";
				return 0;
			}

			if( keynum == KEY_0 || keynum == KEY_PAD_0 )
			{
				g_pConsole->cmdLine += "0";
				return 0;
			}

			if( keynum == KEY_1 || keynum == KEY_PAD_1 )
			{
				g_pConsole->cmdLine += "1";
				return 0;
			}

			if( keynum == KEY_2 || keynum == KEY_PAD_2 )
			{
				g_pConsole->cmdLine += "2";
				return 0;
			}

			if( keynum == KEY_3 || keynum == KEY_PAD_3 )
			{
				g_pConsole->cmdLine += "3";
				return 0;
			}

			if( keynum == KEY_4 || keynum == KEY_PAD_4 )
			{
				g_pConsole->cmdLine += "4";
				return 0;
			}

			if( keynum == KEY_5 || keynum == KEY_PAD_5 )
			{
				g_pConsole->cmdLine += "5";
				return 0;
			}

			if( keynum == KEY_6 || keynum == KEY_PAD_6 )
			{
				g_pConsole->cmdLine += "6";
				return 0;
			}

			if( keynum == KEY_7 || keynum == KEY_PAD_7 )
			{
				g_pConsole->cmdLine += "7";
				return 0;
			}

			if( keynum == KEY_8 || keynum == KEY_PAD_8 )
			{
				g_pConsole->cmdLine += "8";
				return 0;
			}

			if( keynum == KEY_9 || keynum == KEY_PAD_9 )
			{
				g_pConsole->cmdLine += "9";
				return 0;
			}

			if( keynum == KEY_PERIOD )
			{
				g_pConsole->cmdLine += ".";
				return 0;
			}

			if( keynum == KEY_COMMA )
			{
				g_pConsole->cmdLine += ",";
				return 0;
			}

			if( keynum == KEY_SPACE )
			{
				g_pConsole->cmdLine += " ";
				return 0;
			}

			if( keynum == KEY_MINUS )
			{
				g_pConsole->cmdLine += "_";
				return 0;
			}

			if( keynum == KEY_BACKSPACE  )
			{
				if( g_pConsole->cmdLine.length( ) > 0 )
				{
					g_pConsole->cmdLine.erase( g_pConsole->cmdLine.length( ) - 1, g_pConsole->cmdLine.length( ) );
				}
				return 0;
			}

			if( keynum == KEY_DOWN  )
			{
				g_pConsole->lineIndex++;
				if( g_pConsole->lineIndex < g_pConsole->prevLines.size( ) )
				{
					if( g_pConsole->lineIndex >= 0 && ( unsigned int )g_pConsole->lineIndex < g_pConsole->prevLines.size( ) )
					{
						g_pConsole->cmdLine = g_pConsole->prevLines[ g_pConsole->lineIndex ];
					}
				}
				else
				{
					g_pConsole->cmdLine = "";
				}
				g_pConsole->cursorpos = ( char* )g_pConsole->cmdLine.c_str( ) + strlen( g_pConsole->cmdLine.c_str( ) );
				return 0;
			}

			if( keynum == KEY_UP  )
			{
				if( g_pConsole->lineIndex > 0 )
				{
					g_pConsole->lineIndex--;
				}
				if( g_pConsole->lineIndex >= 0 && ( unsigned int )g_pConsole->lineIndex < g_pConsole->prevLines.size( ) )
				{
					g_pConsole->cmdLine = g_pConsole->prevLines[ g_pConsole->lineIndex ];
				}
				g_pConsole->cursorpos = ( char* )g_pConsole->cmdLine.c_str( ) + strlen( g_pConsole->cmdLine.c_str( ) );
				return 0;
			}

			if( keynum == KEY_LEFT || keynum == KEY_RIGHT )
			{
				if( g_pConsole->completed )
				{
					g_pConsole->cmdLine = g_pConsole->iCvar;
				}
			}

			if( keynum == KEY_ENTER || keynum == KEY_PAD_ENTER )
			{
				char buf[ 255 ];
				if( !g_pConsole->cmdLine.empty( ) )
				{
					sprintf( buf,"^w]^a %s", g_pConsole->cmdLine.c_str( ) );
					g_pConsole->scrollCount = 0;
					g_pConsole->lineIndex = g_pConsole->prevLines.size( ) + 1;
					g_pConsole->prevLines.push_back( g_pConsole->cmdLine );
					g_pConsole->CommandHandler( g_pConsole->cmdLine );
					g_pConsole->cmdLine.clear( );
					g_pConsole->cursorpos = ( char* )g_pConsole->cmdLine.c_str( );
				}
				return 0;
			}
			return 0;
		}
	}
	return pKeyEvent( eventcode, keynum, pszCurrentBinding );
}