#ifndef CESP_H
#define CESP_H

#include "stdafx.h"
#include "Globals.h"
#include "Ent.h"
#include "Draw.h"
#include "Math.h"
#include "Stuff.h"

class cESP
{
private:
	Vector vPlayerHead, vPlayerFoot, vecTarget, vPlayerFootScreen, vPlayerHeadScreen, ppVecOrigin, vecScreen, vChickenFootScreen, vChickenHeadScreen, ChickenVecOrigin, vChickenFoot, vChickenHead, vecCurPos;
	bool bDucking;
	Color ESP_COLOR;
	float width, espwidth, espheight, Chickenespwidth, Chickenespheight, Chickenwidth, DrawWidth;
	player_info_t pInfo;
	void HitBoxESP( C_BasePlayer* pTarget, int iHitbox, Color color );
	void DrawHitBox( matrix3x4_t matrix, Vector max, Vector min, Color color );
	bool GetHitboxPosition( int nIndex, cEntity *pEnt, Vector &vOut );

public:
	void DrawPlayer( );
	player_info_t Info;

};
extern cESP pESP;

#endif