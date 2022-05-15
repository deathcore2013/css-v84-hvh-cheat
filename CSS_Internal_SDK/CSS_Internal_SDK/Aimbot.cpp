#include "Aimbot.h"

cAimbot *pAimbot = new cAimbot( );

static void GetMaterialParameters( int iMaterial, float &flPenetrationModifier, float &flDamageModifier )
{
	switch( iMaterial )
	{
	case CHAR_TEX_METAL:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.3;
		break;
	case CHAR_TEX_DIRT:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.3;
		break;
	case CHAR_TEX_CONCRETE:
		flPenetrationModifier = 0.4;
		flDamageModifier = 0.25;
		break;
	case CHAR_TEX_GRATE:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.99;
		break;
	case CHAR_TEX_VENT:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.45;
		break;
	case CHAR_TEX_TILE:
		flPenetrationModifier = 0.65;
		flDamageModifier = 0.3;
		break;
	case CHAR_TEX_COMPUTER:
		flPenetrationModifier = 0.4;
		flDamageModifier = 0.45;
		break;
	case CHAR_TEX_WOOD:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.6;
		break;
	default:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.5;
		break;
	}
}
float DotProduct3(const float *v1, const float *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

VOID cAimbot::VectorTransform2(const Vector in1, const matrix3x4_t& in2, Vector& out)
{
	float buf[3];
	buf[0] = in1.x;
	buf[1] = in1.y;
	buf[2] = in1.z;

	out[0] = DotProduct3(buf, in2[0]) + in2[0][3];
	out[1] = DotProduct3(buf, in2[1]) + in2[1][3];
	out[2] = DotProduct3(buf, in2[2]) + in2[2][3];
}
bool cAimbot::AutoWall( const Vector &vecAbsStart, Vector &vecAbsEnd, cEntity* pLocal, float flDamage )
{
	Ray_t ray;
	trace_t tr;
	CTraceFilter filter;
	filter.pSkip = pLocal;
	cEntity* pBaseEntity = NULL;
	float flTravelledDistance = 0.f,flTempLength = 0.f, flLength, flPenetrationModifier;
	int iLocalID = g_pEngine->GetLocalPlayer( );
	Vector vecDir = ( vecAbsEnd - vecAbsStart ), vecSrc, vecEnd, tmpVec;
	VectorCopy( vecAbsStart, vecSrc );
	flLength = vecDir.Length( );
	vecDir /= flLength;

	DWORD pWeapon = ( DWORD )pLocal->GetCombatWeapon( );
	if( !pWeapon ) return false;

	int iWeapon = pLocal->GetWeaponID( );
	if( iWeapon == 0 ) return false;

	WeaponInfo_s WeaponInfo = GetWeaponInfo( iWeapon, pWeapon, pLocal );

	float flMustDamage = WeaponInfo.fCurrentDamage * flDamage;

	float flDamageModifier = 0.5f;
	while( WeaponInfo.iPenetration || WeaponInfo.fCurrentDamage > 0 )
	{
		vecEnd = vecSrc + vecDir * 8.f;

		if( flTravelledDistance > WeaponInfo.flDistance ) return false;

		ray.Init( vecSrc, vecEnd );
		g_pTrace->TraceRay( ray, 0x4600400B, &filter, &tr );

		if( tr.m_pEnt && tr.m_pEnt->index != iLocalID )
		{
			pBaseEntity = ( cEntity* )tr.m_pEnt;
			filter.pSkip = pBaseEntity;
		}

		g_pTrace->TraceRay( ray, 0x4600400B, &filter, &tr );

		flTravelledDistance += 8.f;

		if( tr.fraction != 1.0f )
		{
			surfacedata_t* pData = g_pPhysics->GetSurfaceData( tr.surface.surfaceProps );
			GetMaterialParameters( pData->game.material, flPenetrationModifier, flDamageModifier );

			VectorSubtract( tr.endpos, vecAbsStart, tmpVec );
			flTempLength = tmpVec.Length( );

			WeaponInfo.fCurrentDamage *= ( pow ( WeaponInfo.flRangeModifier, ( tr.fraction * flTempLength ) / 500 ) );
			WeaponInfo.fCurrentDamage *= flDamageModifier;

			if( 8.f > ( WeaponInfo.flPenetrationPower * flPenetrationModifier ) ) return false;
			if( flTravelledDistance > WeaponInfo.flPenetrationDistance ) return false;

			WeaponInfo.flPenetrationPower -= 8.f / flPenetrationModifier;
			WeaponInfo.flDistance = ( WeaponInfo.flDistance - flTravelledDistance ) * 0.5f;

			if( flTempLength >= flLength )
			{
				if( WeaponInfo.fCurrentDamage >= flMustDamage )
					return true;
				else
					return false;
			}

			WeaponInfo.iPenetration--;
		}
		else
		{
			VectorSubtract( tr.endpos, vecAbsStart, tmpVec );

			flTempLength = tmpVec.Length( );

			if( flTempLength >= flLength )
			{
				if( WeaponInfo.fCurrentDamage >= flMustDamage )
					return true;
				else
					return false;
			}
		}
		vecSrc = vecEnd;
	}
	return false;
}

bool cAimbot::AimbotMain( CUserCmd *pCmd )
{
	g_pEngine->GetViewAngles( ViewAngles );
	ViewAngles.NormalizeAngles( );

	cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if( !pLocal || !pLocal->IsAlive( ) ) return false;

	int Targetid = this->GetBestTarget( );
	if( Targetid < 1 ) return false;

	cEntity *pTarget = ( cEntity* )g_pEntList->GetClientEntity( Targetid );
	if( !pTarget ) return false;

	CalcAngles( vecCurPos, vecTargetPos, vecAimAngles );

	if( pLocal->GetAmmo( ) > 0 )
	{
		pNoSpread->GetSpreadFix( pCmd->random_seed, vecAimAngles, SpreadFix );
		vecAimAngles = vecAimAngles + SpreadFix;
		vecAimAngles.NormalizeAngles( );
		pCmd->viewangles = vecAimAngles;
		return true;
	}
	return false;
}

int cAimbot::GetBestTarget( )
{
	cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if( !pLocal ) return -1;

	int     iAimSpot    = 12;
	int		best		= 0;
	float	besttemp	= 9999;
	vecTargetPos		= Vector( 0.0f, 0.0f, 0.0f );

	for( int Index = 1; Index <= pGlobals->maxClients; Index++ ) 
	{
		cEntity *pIndex = ( cEntity* )g_pEntList->GetClientEntity( Index );
		if( pIndex == pLocal || !pIndex ) continue;
		if( !pIndex->IsValid( ) || pIndex->GetTeam( ) == pLocal->GetTeam( ) ) continue;
		if( sPlayerList[ Index ].bIsFriend ) continue;
		bool bBodyAim = sPlayerList[ Index ].bBodyAim;

		if( pIndex->GetFlags( ) & IN_DUCK ) bBodyAim = true;

		if( pLocal->GetWeaponID( ) == WEAPON_AWP ) iAimSpot = 9;
		if( pLocal->iShots( ) > 2 ) iAimSpot = 11;

		static Vector vecTarget = Vector( 0.0f, 0.0f, 0.0f );

		if( iAimSpot == 12 && !bBodyAim )
		{
			if( !GetHitboxPosition( iAimSpot, pIndex, vecTarget ) ) continue;
			float flPitch = pIndex->GetViewPos( );

			if( ( flPitch > 80.f ) && ( flPitch <= 90.f ) )
			{
				vecTarget.z += 3.0f;
			}
		}

		if( iAimSpot == 11 && !bBodyAim ) 
		{
			if( !GetBonePosition( ( C_BasePlayer* )pIndex, vecTarget, 14 ) ) continue;
		}

		if( iAimSpot == 10 && !bBodyAim ) 
		{
			if( !GetHitboxPosition( iAimSpot, pIndex, vecTarget ) ) continue;
		}

		if( bBodyAim || iAimSpot == 9 )
		{
			if( !GetHitboxPosition( 9, pIndex, vecTarget ) ) continue;
			vecTarget.z += 3.0f;
		}

		vecCurPos = pLocal->GetEyePosition( );
		vCurTargetPos = pIndex->GetEyePosition( );

		ExtraPolation( pLocal, pIndex, vecTarget, vCurTargetPos, Index );

		if( iAimSpot == 12 || iAimSpot == 11 )
		{
			if( !AutoWall( vecCurPos, vecTarget, pLocal, 0.15f ) ) continue;
		}
		else
		{
			if( !AutoWall( vecCurPos, vecTarget, pLocal, 0.25f ) ) continue;
		}

		static QAngle angles = QAngle( 0.0f, 0.0f, 0.0f );
		g_pEngine->GetViewAngles( angles );

		float temp = AngleDif( angles, vecCurPos, vCurTargetPos );
		if( temp < besttemp )
		{
			best = Index;
			besttemp = temp;
			vecTargetPos = vecTarget;
		}
	}
	return best;
}

void cAimbot::ExtraPolation( cEntity *pLocal, cEntity *pEntity, static Vector m_vecPosition, Vector &vEyePos, int Index )
{
	Vector vOld[ 65 ];
	Vector vOldMe;
	Vector vAccel[ 65 ];
	Vector vMyAccel;
	Vector vecEnemyVelocity = pEntity->GetVelocity( );
	Vector vMyVel = pLocal->GetVelocity( );
	Vector vDelta = vecEnemyVelocity - vOld[ Index ];

	vOld[ Index ] = vecEnemyVelocity;
	if( vDelta != Vector( 0.0f, 0.0f, 0.0f ) )
	{
		vAccel[ Index ] = vDelta;
	}

	Vector vDeltaMe = vMyVel - vOldMe;
	vOldMe = vMyVel;

	if( vDeltaMe != Vector( 0.0f, 0.0f, 0.0f ) )
	{
		vMyAccel = vDeltaMe;
	}

	float flTick = pGlobals->interval_per_tick;
	int iFakeLagCmd = sPlayerList[ Index ].flLostTick;

	if( iFakeLagCmd > 5.0f )
	{
		flTick += pGlobals->interval_per_tick * sPlayerList[ Index ].flLostTick;

		if( vAccel[ Index ] != Vector( 0.0f, 0.0f, 0.0f ) || vMyAccel != Vector( 0.0f, 0.0f, 0.0f ) )
		{
			Vector vDivide;
			VectorDivide( ( vAccel[ Index ] - vMyAccel ) * sqrt( flTick ) + ( vecEnemyVelocity - vMyVel ) * flTick + m_vecPosition - vEyePos , 0.5f, vDivide );
			m_vecPosition -= vDivide;
			g_pCvar->FindVar( "cl_interpolate" )->SetValue( "0.695" );
			g_pCvar->FindVar( "cl_updaterate" )->SetValue( "1048576" );
			g_pCvar->FindVar( "cl_cmdrate" )->SetValue( "1048576" ); 
		}
	}

	if( iFakeLagCmd <= 5.0f && iFakeLagCmd > 1.0f )
	{
		flTick += pGlobals->interval_per_tick * sPlayerList[ Index ].flLostTick;

		if( vAccel[ Index ] != Vector( 0.0f, 0.0f, 0.0f ) || vMyAccel != Vector( 0.0f, 0.0f, 0.0f ) )
		{
			Vector vDivide;
			VectorDivide( ( vAccel[ Index ] - vMyAccel ) * sqrt( flTick ) + ( vecEnemyVelocity - vMyVel ) * flTick + m_vecPosition - vEyePos , 0.5f, vDivide );
			m_vecPosition -= vDivide;
			g_pCvar->FindVar( "cl_interpolate" )->SetValue( "0.7" );
			g_pCvar->FindVar( "cl_updaterate" )->SetValue( "66" );
			g_pCvar->FindVar( "cl_cmdrate" )->SetValue( "66" );
		}
	}

	if( iFakeLagCmd == 1.0f )
	{
		if( vAccel[ Index ] != Vector( 0.0f, 0.0f, 0.0f ) || vMyAccel != Vector( 0.0f, 0.0f, 0.0f ) )
		{
			Vector vDivide;
			VectorDivide( ( vAccel[ Index ] - vMyAccel ) * sqrt( flTick ) + ( vecEnemyVelocity - vMyVel ) * flTick + m_vecPosition - vEyePos , 0.5f, vDivide );
			m_vecPosition += vDivide;
			g_pCvar->FindVar( "cl_updaterate" )->SetValue( "67" );
			g_pCvar->FindVar( "cl_cmdrate" )->SetValue( "67" );
			g_pCvar->FindVar( "cl_interpolate" )->SetValue( "1" );
		}
	}
}

bool cAimbot::GetHitboxPosition( int nIndex, cEntity *pEnt, Vector &vOut )
{
	matrix3x4_t		pMatrix;
	Vector			vMin, vMax, Bone;
	mstudiobbox_t*	box = NULL;
	const model_t*	pModel = pEnt->GetModel( );
	studiohdr_t*	pStudioHdr = g_pModelInfo->GetStudiomodel( pModel );

	box = pStudioHdr->pHitbox( nIndex, 0 );
	if( !GetBonePosition( ( C_BasePlayer* )pEnt, Bone, box->bone ) ) return false;
	DWORD dwBoneMatrix = *( DWORD* )( pEnt + cBoneMatrix );
	pMatrix = *(matrix3x4_t*)(dwBoneMatrix + 0x770 * box->bone); //либо 0x570 незнаю 

	VectorTransform2( box->bbmin, pMatrix, vMin );
	VectorTransform2( box->bbmax, pMatrix, vMax );
	vOut = ( vMin + vMax ) * 0.5f;

	return true;
}

Vector cAimbot::HitScans( cEntity *pLocal, cEntity *pTarget )
{
	for( int i = 0; i < 44; i++ )
	{
		Vector vBone = pTarget->GetBonePosition( i );
		if( IsVisible( pLocal->GetEyePosition( ), vBone, pLocal, pTarget ) )
		{
			return vBone;
		}
	}
	return Vector( 0.0f, 0.0f, 0.0f );
}