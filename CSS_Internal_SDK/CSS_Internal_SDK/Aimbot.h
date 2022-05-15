#include "stdafx.h"
#include "Ent.h"
#include "Math.h"
#include "MD5.h"
#include "Nospread.h"
#include "Stuff.h"
#include "defines.h"

class cAimbot 
{
public:
	int		GetBestTarget( );
	bool	AimbotMain( CUserCmd *pCmd );
	bool    GetHitboxPosition( int nIndex, cEntity *pEnt, Vector &vOut );
	VOID VectorTransform2(const Vector in1, const matrix3x4_t& in2, Vector& out);
private:
	QAngle	ViewAngles, SpreadFix, vecAimAngles;
	Vector  vecTargetPos, vecCurPos, vCurTargetPos;
	void    ExtraPolation( cEntity *pLocal, cEntity *pEntity, static Vector m_vecPosition, Vector &vEyePos, int Index );
	bool    AutoWall( const Vector &vecAbsStart, Vector &vecAbsEnd, cEntity* pLocal, float flDamage );
	Vector  HitScans( cEntity *pLocal, cEntity *pTarget );
};

extern cAimbot *pAimbot;