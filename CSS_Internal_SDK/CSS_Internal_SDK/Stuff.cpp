#include "Stuff.h"

bool IsVisible( Vector vStart, Vector vEnd, cEntity *pLocal, cEntity *pEnt )
{
	Ray_t ray;	
	ray.Init( vStart , vEnd );	

	trace_t tr;	
	CTraceFilter tf; 
	tf.pSkip = pLocal;

	g_pTrace->TraceRay( ray, MASK_SHOT, &tf, &tr );

	if( pEnt && tr.m_pEnt )
	{
		cEntity *pTraceEnt = ( cEntity* )g_pEntList->GetClientEntity( tr.m_pEnt->index );
		return( pTraceEnt == pEnt || tr.fraction >= 1.0f );
	}

	return( tr.fraction >= 1.0f );
}

void pPred( CUserCmd *pCmd, C_BasePlayer *pLocal )
{
	float flOldCurtime = pGlobals->curtime;
	float flOldFrametime = pGlobals->frametime;

	int iTickBase = *(int*)((DWORD)pLocal + 0x10ec); //вроде обновлен

	pGlobals->curtime = iTickBase * pGlobals->interval_per_tick;
	pGlobals->frametime = pGlobals->interval_per_tick;

	BYTE moveData[ 0xA4 ];
	CUserCmd* oldCmd = *( CUserCmd** )( ( DWORD )pLocal + 0x100C );
	*( CUserCmd** )( ( DWORD )pLocal + 0x100C ) = pCmd;

	g_pPred->SetupMove( pLocal, pCmd, NULL, &moveData );
	g_pGameMovement->ProcessMovement( pLocal, &moveData );
	g_pPred->FinishMove( pLocal, pCmd, &moveData );

	*( CUserCmd** )( ( DWORD )pLocal + 0x100C ) = oldCmd;

	pGlobals->curtime = flOldCurtime;
	pGlobals->frametime = flOldFrametime;
}

QAngle DecayPunchangle( QAngle vPunchAngle )
{
	float len = _QAngleNormalize( vPunchAngle );
	len -= ( 10.0 + len * 0.5 ) * pGlobals->interval_per_tick;
	len = max( len, 0.0 );
	vPunchAngle *= len;
	return vPunchAngle;
}

void FakeLaggPred( )
{
	cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if( !pLocal ) return;

	for( int Index = 1; Index <= pGlobals->maxClients; Index++ ) 
	{
		cEntity *pIndex = ( cEntity* )g_pEntList->GetClientEntity( Index );
		if( pIndex == pLocal || !pIndex ) continue;
		if( !pIndex->IsValid( ) || pIndex->GetTeam( ) == pLocal->GetTeam( ) ) continue;

		float flSimTime = pIndex->flSimulationTime( );

		if( sPlayerList[ Index ].old_flTickCount == flSimTime )
		{
			sPlayerList[ Index ].flLostTick += 1.0f;
		}
		else
		{
			sPlayerList[ Index ].flLostTick = 0.0f;
		}

		sPlayerList[ Index ].old_flTickCount = flSimTime;

		if( sPlayerList[ Index ].flLostTick >= 15.0f ) sPlayerList[ Index ].flLostTick = 0;
	}
}

void FixXRegular( const CRecvProxyData *pData, void *pStruct, void *pOut ) 
{
	float pitch = pData->m_Value.m_Float;

	if( sPlayerList[ ( ( C_BasePlayer* )pStruct, ( ( C_BasePlayer* )( pStruct ) )->index ) ].bMagicPitch )
	{
		pitch = 271.0f;
		*( float* )( pOut ) = pitch;
	}
	else
	{
		if ( ( pitch > 180.087936f )
			&& ( pitch < 271.0f ) )
		{
			pitch = 271.0f;
		}
		else if ( ( pitch >= 89.0f )
			&& ( pitch <= 180.087936f ) )
		{
			pitch = 89.0f;
		}	

		*( float* )( pOut ) = pitch;
	}
}

void FixYRegular( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	static Vector last[ MAX_PLAYERS ];
	static bool shotLastTime[ MAX_PLAYERS ];

	float *pitch = ( float* )( ( DWORD )pOut - 4 );

	float yaw = pData->m_Value.m_Float;

	bool hasAA = ( ( *pitch == 90.0f ) || ( *pitch == 270.0f ) );
	bool spinbot = false;

	if ( !shotLastTime[ ( ( C_BasePlayer* )( pStruct ) )->index ]
	&& ( fabsf( yaw - last[ ( ( C_BasePlayer* )( pStruct ) )->index ].y ) > 15.0f ) && !hasAA )
	{
		yaw = last[ ( ( C_BasePlayer* )( pStruct ) )->index ].y;

		shotLastTime[ ( ( C_BasePlayer* )( pStruct ) )->index ] = true;
	}
	else
	{
		if ( shotLastTime[ ( ( C_BasePlayer* )( pStruct ) )->index ]
		&& ( fabsf( yaw - last[ ( ( C_BasePlayer* )( pStruct ) )->index ].y ) > 15.0f ) )
		{
			shotLastTime[ ( ( C_BasePlayer* )( pStruct ) )->index ] = true;
			spinbot = true;
		}
		else
		{
			shotLastTime[ ( ( C_BasePlayer* )( pStruct ) )->index ] = false;
		}
	}

	last[ ( ( C_BasePlayer* )( pStruct ) )->index ].y = yaw;

	static bool jittersfix[ MAX_PLAYERS ];
	bool tmp = jittersfix[ ( ( C_BasePlayer* )( pStruct ) )->index ];

	jittersfix[ ( ( C_BasePlayer* )( pStruct ) )->index ] = ( yaw >= 180 && yaw <= 360 );

	if( tmp && ( yaw >= 0 && yaw <= 180 ) )
	{
		yaw += 359;
	}

	*( float* )( pOut ) = yaw;
}

std::vector<IMaterial*> vecSkyTextures;
std::vector<IMaterial*> vecPlayerTextures;
std::vector<IMaterial*> vecWeaponTextures;
std::vector<IMaterial*> vecMapTextures;

void SetAsusWalls( float flAlpha )
{
	for( unsigned int i = 0; i < vecMapTextures.size( ); i++ )
	{
		vecMapTextures[ i ]->SetMaterialVarFlag( MATERIAL_VAR_IGNORE_ALPHA_MODULATION, false );
		vecMapTextures[ i ]->SetMaterialVarFlag( MATERIAL_VAR_ALLOWALPHATOCOVERAGE, true );
		vecMapTextures[ i ]->SetMaterialVarFlag( MATERIAL_VAR_FLAT, true );
		vecMapTextures[ i ]->AlphaModulate( flAlpha );
		vecMapTextures[ i ]->ColorModulate( 0.89f, 0.89f, 0.89f );
	}
}

void SetSkyChams( bool bDraw )
{
	for( unsigned int i = 0; i < vecSkyTextures.size( ); i++ )
	{
		if( !bDraw )
			vecSkyTextures[ i ]->ColorModulate( 0.0001f, 0.0001f, 0.0001f );
	}
}

void SetPlayerChams( bool bWallHack, bool bWireFrame )
{
	for( unsigned int i = 0; i < vecPlayerTextures.size( ); i++ )
	{
		vecPlayerTextures[ i ]->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, bWallHack );
		vecPlayerTextures[ i ]->SetMaterialVarFlag( MATERIAL_VAR_ZNEARER, bWallHack );
		vecPlayerTextures[ i ]->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, bWireFrame );
	}
}

void SetWeaponChams( bool bWireFrame, bool bNoHand )
{
	for( unsigned int i = 0; i < vecWeaponTextures.size( ); i++ )
	{
		if( strstr( vecWeaponTextures[ i ]->GetName( ), "hand" ) )
			vecWeaponTextures[ i ]->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, bNoHand );
		else
			vecWeaponTextures[ i ]->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, bWireFrame );
	}
}

bool GetBonePosition( C_BasePlayer *pTarget, Vector& vBonePos, int iBone )
{
	static matrix3x4_t Matrix[ 128 ];

	if( !pTarget->SetupBones( Matrix, 128, 0x00000100, 0.0f ) ) return false;

	vBonePos.x = Matrix[ iBone ][ 0 ][ 3 ];
	vBonePos.y = Matrix[ iBone ][ 1 ][ 3 ];
	vBonePos.z = Matrix[ iBone ][ 2 ][ 3 ];

	return true;
}

void ResetPlayerList( )
{
	for( int i = 0; i < 64; i++ )
	{
		sPlayerList[ i ].bIsFriend = false;
		sPlayerList[ i ].bMagicPitch = false;
		sPlayerList[ i ].bBodyAim = false;
	}
}

const char *GetWeaponAlias( int weaponID )
{
	typedef const char* ( *WeaponIDToAlias_t )( int id );
	static WeaponIDToAlias_t WeaponIDToAlias = (WeaponIDToAlias_t)((DWORD)dwClient + 0x20E6D0);
	return WeaponIDToAlias( weaponID );
}

WEAPON_FILE_INFO_HANDLE LookupWeaponInfoSlot( const char *name )
{
	typedef WEAPON_FILE_INFO_HANDLE (*LookupWeaponInfoSlot_t) ( const char *name );
	static LookupWeaponInfoSlot_t LookupWeaponInfoSlot = (LookupWeaponInfoSlot_t)((DWORD)dwClient + 0x1C3A70);
	return LookupWeaponInfoSlot( name );
}

FileWeaponInfo_t *GetFileWeaponInfoFromHandle( WEAPON_FILE_INFO_HANDLE handle )
{
	typedef FileWeaponInfo_t* (*GetFileWeaponInfoFromHandle_t)( WEAPON_FILE_INFO_HANDLE handle );
	static GetFileWeaponInfoFromHandle_t GetFileWeaponInfoFromHandle = (GetFileWeaponInfoFromHandle_t)((DWORD)dwClient + 0x1C3390);
	return GetFileWeaponInfoFromHandle( handle );
}

FileWeaponInfo_t *GetSDKWeaponInfo( int iWeapon )
{
	char szPropName[ 128 ];
	sprintf_s( szPropName, "weapon_%s", GetWeaponAlias( iWeapon ) );

	WEAPON_FILE_INFO_HANDLE	hWpnInfo = LookupWeaponInfoSlot( szPropName );

	FileWeaponInfo_t *pWeaponInfo = GetFileWeaponInfoFromHandle( hWpnInfo );

	return pWeaponInfo;
}

WeaponInfo_s GetWeaponInfo( int iWeaponID, DWORD pWeapon, cEntity *pLocal )
{
	FileWeaponInfo_t *sdkWeaponInfo = GetSDKWeaponInfo( iWeaponID );

	WeaponInfo_s weaponInfo;

	weaponInfo.iPenetration = *reinterpret_cast<int*>( (size_t)sdkWeaponInfo + 0x884 );
	weaponInfo.fCurrentDamage = *reinterpret_cast<int*>((size_t)sdkWeaponInfo + 0x88c); //обновлен вроде
	weaponInfo.flDistance = *reinterpret_cast<float*>((size_t)sdkWeaponInfo + 0x28); //обновлен вроде
	weaponInfo.flRangeModifier = *reinterpret_cast<float*>((size_t)sdkWeaponInfo + 0x1438); // наверно обновлен
	weaponInfo.iBulletsPerShot = *reinterpret_cast<int*>((size_t)sdkWeaponInfo + 0x974); //обновлен наверно 
	weaponInfo.iAmmoType = *reinterpret_cast<int*>((size_t)sdkWeaponInfo + 0xc40); //обновлен наверно
	weaponInfo.iWeaponID = iWeaponID;

	if( weaponInfo.iWeaponID == WEAPON_M4A1 )
	{
		bool specialWeaponMode = *reinterpret_cast<bool*>((size_t)pWeapon + 0x92c); //обновлен вроде

		if( specialWeaponMode == 1 )
		{
			weaponInfo.flRangeModifier = .95f;
		}
	}

	if( weaponInfo.iWeaponID == WEAPON_GLOCK )
	{
		bool specialWeaponMode = *reinterpret_cast<bool*>((size_t)pWeapon + 0x92c); //обновлен

		if( specialWeaponMode )
		{
			int burstShotsRemaining = *reinterpret_cast<int*>( (size_t)pWeapon + 0x95C );

			if( burstShotsRemaining > 0 )
			{
				weaponInfo.fCurrentDamage = 18;
				weaponInfo.flRangeModifier = .9f;
			}
		}
	}

	if( weaponInfo.iWeaponID == WEAPON_USP45 )
	{
		bool specialWeaponMode = *reinterpret_cast<bool*>( (size_t)pWeapon + 0x92c ); //обновлен

		if( specialWeaponMode )
		{
			weaponInfo.fCurrentDamage = 30;
		}
	}

	pNoSpread->GetBulletTypeInformation( weaponInfo.iAmmoType, weaponInfo.flPenetrationPower, weaponInfo.flPenetrationDistance );

	return weaponInfo;
}

bool EdgeAntiAim( cEntity *localPlayer, CUserCmd *cmd , float angle )
{
	bool ret = false;

	Vector localPosition = localPlayer->GetEyePosition( );

	for( int y = 0; y < 360; y++ )
	{
		QAngle tmp( 10.f, cmd->viewangles.y, 0.f );
		tmp.y += y;

		tmp.NormalizeAngles( );

		Vector forward;
		AngleVectors( tmp, &forward );

		float length = ( ( 16 + 3 ) + ( ( 16 + 3 ) * sin( DegToRad( 10.f ) ) ) ) + 5;

		forward *= length;

		Ray_t ray;
		trace_t traceData;	

		ray.Init( localPosition, ( localPosition + forward ) );
		g_pTrace->TraceRay( ray, 0x200400B, NULL, &traceData );

		if( traceData.fraction != 1.f )
		{
			QAngle angles;
			Vector Negate = traceData.plane.normal;
			Negate *= -1;
			VectorAngles( Negate, angles );

			tmp.y = angles.y;

			tmp.NormalizeAngles( );

			trace_t leftTrace, rightTrace;

			Vector left, right;
			AngleVectors( tmp + QAngle( 0.f, 30.f, 0.f ), &left );
			AngleVectors( tmp - QAngle( 0.f, 30.f, 0.f ), &right );

			left *= ( length + ( length * sin( DegToRad( 30.f ) ) ) );
			right *= ( length + ( length * sin( DegToRad( 30.f ) ) ) );


			ray.Init( localPosition, ( localPosition + left ) );
			g_pTrace->TraceRay( ray, 0x200400B, NULL, &leftTrace );


			ray.Init( localPosition, ( localPosition + right ) );
			g_pTrace->TraceRay( ray, 0x200400B, NULL, &rightTrace );


			if ( ( leftTrace.fraction == 1.f )
				&& ( rightTrace.fraction != 1.f ) )
			{
				//tmp.y += 90.f-(45.f/2.f);
				//tmp.y -= 45.f;
				tmp.y -= 90.f;
				// LEFT
			}
			else if ( ( leftTrace.fraction != 1.f )
				&& ( rightTrace.fraction == 1.f ) )
			{
				//tmp.y -= 90.f-(45.f/2.f);
				//tmp.y += 45.f;
				tmp.y += 90.f;
				// RIGHT
			}

			cmd->viewangles.y = tmp.y;
			cmd->viewangles.y -= angle;
			ret = true;
		}
	}

	return ret;
}

float GetAntiAimAngles( )
{
	cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if( !pLocal ) return -1;

	int     best        = 0;
	float	besttemp	= 9999;
	QAngle  vAntiAim	= QAngle( 0, -90, 0 );

	for( int Index = 1; Index <= pGlobals->maxClients; Index++ ) 
	{
		cEntity *pIndex = ( cEntity* )g_pEntList->GetClientEntity( Index );
		if( pIndex == pLocal || !pIndex ) continue;
		if( !pIndex->IsValid( ) || pIndex->GetTeam( ) == pLocal->GetTeam( ) ) continue;

		Vector vMyPos = pLocal->GetAbsOrigin( );
		Vector vTarget = pIndex->GetAbsOrigin( );

		float temp = vMyPos.DistTo( vTarget );

		if( temp < besttemp )
		{
			best = Index;
			besttemp = temp;
			CalcAngles( vMyPos, vTarget, vAntiAim );
			vAntiAim.NormalizeAngles( );
		}
	}
	return vAntiAim.y - 180;
}

BYTE m_MoveType( cEntity *pLocal )
{ 
	return *( BYTE* )( ( DWORD )pLocal + 0x174 );
}

float GetAntiAimAngles2( )
{
	cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if( !pLocal ) return -1;

	int     best        = 0;
	float	besttemp	= 9999;
	QAngle  vAntiAim	= QAngle( 0, -90, 0 );

	for( int Index = 1; Index <= pGlobals->maxClients; Index++ ) 
	{
		cEntity *pIndex = ( cEntity* )g_pEntList->GetClientEntity( Index );
		if( pIndex == pLocal || !pIndex ) continue;
		if( !pIndex->IsValid( ) || pIndex->GetTeam( ) == pLocal->GetTeam( ) ) continue;

		Vector vMyPos = pLocal->GetAbsOrigin( );
		Vector vTarget = pIndex->GetAbsOrigin( );

		float temp = vMyPos.DistTo( vTarget );

		if( temp < besttemp )
		{
			best = Index;
			besttemp = temp;
			CalcAngles( vMyPos, vTarget, vAntiAim );
			vAntiAim.NormalizeAngles( );
		}
	}
	return vAntiAim.y;
}