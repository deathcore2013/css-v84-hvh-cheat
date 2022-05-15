#ifndef CNOSPREAD_H
#define CNOSPREAD_H

#include "stdafx.h"
#include "Globals.h"
#include "Ent.h"
#include "Random.h"
#include "Math.h"

class NoSpread
{
public:
	void  GetSpreadFix( int iSeed, QAngle& vIn, QAngle& vOut );
	void  UpdateAccuracyPenalty( DWORD *pWeapon );
	float GetWeaponSpread( DWORD pWeapon );
	float GetWeaponCone( DWORD pWeapon );
	void  UpdateAccuracyPenalty( DWORD pWeapon );
	void  ItSpread( int random_seed, Vector& out );
	void  GetWeaponData( int iWeaponID, int &iBulletType, int &iPenetration, int &iDamage, float &flDistance, float &flRangeModifier );
	void  GetBulletTypeInformation( int iBulletType, float &flPenetrationPower, float &flPenetrationDistance );
};

extern NoSpread *pNoSpread;

#endif