#include "Nospread.h"
#include "Stuff.h"

NoSpread *pNoSpread = new NoSpread( );

void NoSpread::UpdateAccuracyPenalty( DWORD pWeapon )
{
	typedef void ( __thiscall *UpdateAccuracyPenalty_t )( void *thisptr );
	reinterpret_cast< UpdateAccuracyPenalty_t >(*reinterpret_cast< PDWORD >(*reinterpret_cast< PDWORD >(pWeapon)+0x5E8))((void*)pWeapon);
}

float NoSpread::GetWeaponSpread( DWORD pWeapon )
{
	typedef float ( __thiscall *GetWeaponSpread_t )( void *thisptr );
	return reinterpret_cast< GetWeaponSpread_t >(*(size_t*)(*(size_t*)(pWeapon)+0x5E0))((void*)pWeapon);
}

float NoSpread::GetWeaponCone( DWORD pWeapon )
{
	typedef float ( __thiscall *GetSpreadCone_t )( void *thisptr );
	return reinterpret_cast< GetSpreadCone_t >(*reinterpret_cast< PDWORD >(*reinterpret_cast< PDWORD >(pWeapon)+0x5E4))((void*)pWeapon);
}

void NoSpread::GetSpreadFix( int iSeed, QAngle& vIn, QAngle& vOut )
{
	Vector forward, right, up, vecDir, spread;
	QAngle dest;

	AngleVectors( vIn, &forward, &right, &up );
	ItSpread( iSeed, spread );

	vecDir.x = forward.x + spread.x * right.x + spread.y * up.x;
	vecDir.y = forward.y + spread.x * right.y + spread.y * up.y;
	vecDir.z = forward.z + spread.x * right.z + spread.y * up.z;
	VectorAngles( vecDir, dest );

	vOut.x = vIn.x - dest.x;
	vOut.y = vIn.y - dest.y;
	vOut.z = 0;
}

void NoSpread::ItSpread( int random_seed, Vector& out )
{
	cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if( !pLocal ) return;

	cEntity *pWeapon = pLocal->GetCombatWeapon( );
	if( !pWeapon ) return;

	UpdateAccuracyPenalty( ( DWORD )pWeapon );

	Random.SetSeed( ( random_seed + 1 ) & 255 );

	float fSpread = GetWeaponSpread( ( DWORD )pWeapon );
	float fInaccuracy = GetWeaponCone( ( DWORD )pWeapon );

	WeaponInfo_s m_WeaponInfo = GetWeaponInfo( pLocal->GetWeaponID( ), ( DWORD )pLocal->GetCombatWeapon( ), pLocal );

	float flTempLength = 0.0f;

	static Vector vSpread = Vector( 0.0f, 0.0f, 0.0f );

	for( int i = 0; i < m_WeaponInfo.iBulletsPerShot; i++ )
	{
		float fA = Random.RandomFloat( 0.0f, 2.0f * M_PI );
		float fB = Random.RandomFloat( 0.0f, fSpread );
		float fC = Random.RandomFloat( 0.0f, 2.0f * M_PI );
		float fD = Random.RandomFloat( 0.0f, fInaccuracy );

		vSpread.x = ( cos ( fA ) * fB ) + ( cos ( fC ) * fD );
		vSpread.y = ( sin ( fA ) * fB ) + ( sin ( fC ) * fD );

		flTempLength = vSpread.Length2D( );

		if( flTempLength < 8192.0f )
		{
			out.x = vSpread.x;
			out.y = vSpread.y;
		}
	}
}

void NoSpread::GetBulletTypeInformation( int iBulletType, float &flPenetrationPower, float &flPenetrationDistance )
{
	switch (iBulletType)
	{
	case 0:
		flPenetrationPower = 0;
		flPenetrationDistance = 0;
		break;
	case 1:
		flPenetrationPower = 30.f;
		flPenetrationDistance = 1000.f;
		break;
	case 2:
		flPenetrationPower = 39.f;
		flPenetrationDistance = 5000.f;
		break;
	case 3:
		flPenetrationPower = 35.f;
		flPenetrationDistance = 4000.f;
		break;
	case 4:
		flPenetrationPower = 35.f;
		flPenetrationDistance = 4000.f;
		break;
	case 5:
		flPenetrationPower = 45.f;
		flPenetrationDistance = 8000.f;
		break;
	case 6:
		flPenetrationPower = 21.f;
		flPenetrationDistance = 800.f;
		break;
	case 7:
		flPenetrationPower = 0.f;
		flPenetrationDistance = 0.f;
		break;
	case 8:
		flPenetrationPower = 15.f;
		flPenetrationDistance = 500.f;
		break;
	case 9:
		flPenetrationPower = 25.f;
		flPenetrationDistance = 800.f;
		break;
	case 10:
		flPenetrationPower = 30.f;
		flPenetrationDistance = 2000.f;
		break;
	default:
		flPenetrationPower = 0;
		flPenetrationDistance = 0;
		break;
	}
}

void NoSpread::GetWeaponData( int iWeaponID, int &iBulletType, int &iPenetration, int &iDamage, float &flDistance, float &flRangeModifier )
{
	switch( iWeaponID )
	{
	case 0:
		iBulletType = 0;
		iPenetration = 0;
		iDamage = 0;
		flDistance = 0.0f;
		flRangeModifier = 0.0f;
		break;
	case WEAPON_P228:
		iBulletType = 9;
		iPenetration = 1;
		iDamage = 40;
		flDistance = 4096.0f;
		flRangeModifier = 0.8f;
		break;
	case WEAPON_GLOCK:
		iBulletType = 8;
		iPenetration = 1;
		iDamage = 25;
		flDistance = 8192.0f;
		flRangeModifier = 0.75f;
		break;
	case WEAPON_SCOUT:
		iBulletType = 2;
		iPenetration = 1;
		iDamage = 75;
		flDistance = 8192.0f;
		flRangeModifier = 0.98f;
		break;
	case WEAPON_MAC10:
		iBulletType = 8;
		iPenetration = 1;
		iDamage = 29;
		flDistance = 4096.0f;
		flRangeModifier = 0.82f;
		break;
	case WEAPON_AUG:
		iBulletType = 2;
		iPenetration = 2;
		iDamage = 32;
		flDistance = 8192.0f;
		flRangeModifier = 0.96f;
		break;
	case WEAPON_ELITE:
		iBulletType = 6;
		iPenetration = 1;
		iDamage = 45;
		flDistance = 4096.0f;
		flRangeModifier = 0.75f;
		break;
	case WEAPON_FIVE7:
		iBulletType = 10;
		iPenetration = 1;
		iDamage = 25;
		flDistance = 4096.0f;
		flRangeModifier = 0.885f;
		break;
	case WEAPON_UMP45:
		iBulletType = 8;
		iPenetration = 1;
		iDamage = 30;
		flDistance = 4096.0f;
		flRangeModifier = 0.82f;
		break;
	case WEAPON_SG550:
		iBulletType = 3;
		iPenetration = 2;
		iDamage = 70;
		flDistance = 8192.0f;
		flRangeModifier = 0.98f;
		break;
	case WEAPON_GALIL:
		iBulletType = 3;
		iPenetration = 2;
		iDamage = 30;
		flDistance = 8192.0f;
		flRangeModifier = 0.98f;
		break;
	case WEAPON_FAMAS:
		iBulletType = 3;
		iPenetration = 2;
		iDamage = 30;
		flDistance = 8192.0f;
		flRangeModifier = 0.96f;
		break;
	case WEAPON_USP45:
		iBulletType = 8;
		iPenetration = 1;
		iDamage = 34.0f;
		flDistance = 4096.0f;
		flRangeModifier = 0.79f;
		break;
	case WEAPON_AWP:
		iBulletType = 5;
		iPenetration = 3;
		iDamage = 115;
		flDistance = 8192.0f;
		flRangeModifier = 0.99f;
		break;
	case WEAPON_MP5:
		iBulletType = 6;
		iPenetration = 1;
		iDamage = 26;
		flDistance = 4096.0f;
		flRangeModifier = 0.84f;
		break;
	case WEAPON_M249:
		iBulletType = 4;
		iPenetration = 2;
		iDamage = 32;
		flDistance = 8192.0f;
		flRangeModifier = 0.97f;
		break;
	case WEAPON_M4A1:
		iBulletType = 3;
		iPenetration = 2;
		iDamage = 33;
		flDistance = 8192.0f;
		flRangeModifier = 0.97f;
		break;
	case WEAPON_TMP:
		iBulletType = 6;
		iPenetration = 1;
		iDamage = 26;
		flDistance = 4096.0f;
		flRangeModifier = 0.84f;
		break;
	case WEAPON_G3SG1:
		iBulletType = 2;
		iPenetration = 3;
		iDamage = 80;
		flDistance = 8192.0f;
		flRangeModifier = 0.98f;
		break;
	case WEAPON_DEAGLE:
		iBulletType = 1;
		iPenetration = 2;
		iDamage = 54;
		flDistance = 4096.0f;
		flRangeModifier = 0.81f;
		break;
	case WEAPON_SG552:
		iBulletType = 3;
		iPenetration = 2;
		iDamage = 33;
		flDistance = 8192.0f;
		flRangeModifier = 0.955f;
		break;
	case WEAPON_AK47:
		iBulletType = 2;
		iPenetration = 2;
		iDamage = 36;
		flDistance = 8192.0f;
		flRangeModifier = 0.98f;
		break;
	case WEAPON_P90:
		iBulletType = 10;
		iPenetration = 1;
		iDamage = 26;
		flDistance = 4096.0f;
		flRangeModifier = 0.84f;
		break;
	default:
		iBulletType = 0;
		iPenetration = 0;
		iDamage = 0;
		flDistance = 0.0f;
		flRangeModifier = 0.0f;
		break;
	}
}