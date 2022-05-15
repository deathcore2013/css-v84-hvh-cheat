#ifndef CBASEENTITY_H
#define CBASEENTITY_H

#include "stdafx.h"
#include "Globals.h"
#include "cEngine.h"

class cEntity
{
public:
	cEntity	*GetCombatWeapon( void );
	bool	IsValid( void );
	Vector	GetAbsOrigin( void );
	Vector  GetNormalAbsOrigin( void );
	Vector	GetEyePosition( void );
	Vector  GetNormalEyePosition( void );
	BOOL	IsDormant( void );
	BYTE	GetFlags( void );
	BYTE	GetLifestate( void );
	int		GetHealth( void );
	int		GetTeam( void );
	Vector	GetBonePosition( int iBone );
	int		GetFOV( void );
	int		GetWeaponID( void );
	int		GetAmmo( void );
	char	*GetWeaponName( void );
	bool	IsDefusing( void );
	bool	IsAlive( void );
	QAngle	GetPunchAngles( void );
	int		iShots( void );
	Vector	GetVelocity( void );
	model_t *GetModel( void );
	bool    CanShoot( void );
	float   flSimulationTime( void );
	Color   GetColor( bool alpha );
	float   GetViewPos( );
};
#endif