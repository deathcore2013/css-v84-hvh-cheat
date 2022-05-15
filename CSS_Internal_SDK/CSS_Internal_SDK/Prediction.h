#ifndef _Prediction_
#define _Prediction_

#include "stdafx.h"

class IPrediction
{
public:

	void SetupMove( C_BasePlayer *player, CUserCmd *ucmd, void *pHelper, void *move )
	{
		typedef void ( __thiscall* OriginalFn)( PVOID, C_BasePlayer*, CUserCmd*, void*, void* );
		CallVirtual< OriginalFn >( this, 18 )( this, player, ucmd, pHelper, move );
	}

	void FinishMove( C_BasePlayer *player, CUserCmd *ucmd, void *move )
	{
		typedef void ( __thiscall* OriginalFn)( PVOID, C_BasePlayer*, CUserCmd*, void* );
		CallVirtual< OriginalFn >( this, 19 )( this, player, ucmd, move );
	}
};

class IGameMovement
{
public:
	virtual void ficken( void* x ) = 0;	
	// Process the current movement command
	virtual void	ProcessMovement( C_BasePlayer *pPlayer, void *pMove ) = 0;		
	virtual void	StartTrackPredictionErrors( C_BasePlayer *pPlayer ) = 0;
	virtual void	FinishTrackPredictionErrors( C_BasePlayer *pPlayer ) = 0;
	virtual void	DiffPrint( char const *fmt, ... ) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual Vector const&	GetPlayerMins( bool ducked ) const = 0;
	virtual Vector const&	GetPlayerMaxs( bool ducked ) const = 0;
	virtual Vector const&   GetPlayerViewOffset( bool ducked ) const = 0;

};

#endif