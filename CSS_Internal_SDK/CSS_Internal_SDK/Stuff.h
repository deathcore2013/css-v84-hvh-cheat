#include "stdafx.h"
#include "Globals.h"
#include "Ent.h"
#include "Math.h"
#include "cEngine.h"
#include "NoSpread.h"

#include <vector>
#include <string>

extern bool IsVisible( Vector vStart, Vector vEnd, cEntity *pLocal, cEntity *pEnt );
extern void pPred( CUserCmd *pCmd, C_BasePlayer *pLocal );
extern QAngle DecayPunchangle( QAngle vPunchAngle );
extern void FakeLaggPred( );
extern void FixXRegular( const CRecvProxyData *pData, void *pStruct, void *pOut );
extern void FixYRegular( const CRecvProxyData *pData, void *pStruct, void *pOut );
extern std::vector<IMaterial*> vecSkyTextures;
extern std::vector<IMaterial*> vecPlayerTextures;
extern std::vector<IMaterial*> vecWeaponTextures;
extern std::vector<IMaterial*> vecMapTextures;
extern void SetAsusWalls( float flAlpha );
extern void SetSkyChams( bool bDraw );
extern void SetPlayerChams( bool bWallHack, bool bWireFrame );
extern void SetWeaponChams( bool bWireFrame, bool bNoHand );
extern bool GetBonePosition( C_BasePlayer *pTarget, Vector& vBonePos, int iBone );
extern void ResetPlayerList( );
extern WeaponInfo_s GetWeaponInfo( int iWeaponID, DWORD pWeapon, cEntity *pLocal );
bool EdgeAntiAim( cEntity *localPlayer, CUserCmd *cmd , float angle );
float GetAntiAimAngles( );
extern BYTE m_MoveType( cEntity *pLocal ); 
extern float GetAntiAimAngles2( );