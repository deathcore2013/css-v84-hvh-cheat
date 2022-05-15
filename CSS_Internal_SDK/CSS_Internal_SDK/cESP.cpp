#include "cESP.h"

cESP pESP;

void cESP::DrawPlayer( )
{
	cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if( !pLocal ) return;

	for( int Index = 1; Index <= pGlobals->maxClients; Index++ )
	{
		cEntity *pEnt = ( cEntity* )g_pEntList->GetClientEntity( Index );
		if( !pEnt || pEnt == pLocal ) continue;
		if( !pEnt->IsValid( ) /*|| pEnt->GetTeam( ) == pLocal->GetTeam( )*/ ) continue;
		if( !g_pEngine->GetPlayerInfo( Index, &pInfo ) ) continue;

		vecCurPos = pEnt->GetEyePosition( );

		if( !GetHitboxPosition( 12, pEnt, vecTarget ) ) continue;
		if( pEnt->GetTeam( ) == 2 )	ESP_COLOR = Color( 255, 48, 48, 255 );//t
		if( pEnt->GetTeam( ) == 3 )	ESP_COLOR = Color( 30, 144, 255, 255 );//ct
		//if( IsVisible( vecCurPos, vecTarget, pLocal, pEnt ) ) ESP_COLOR = Color( 0, 255, 0, 255 );//vis check

		vPlayerFoot = pEnt->GetNormalAbsOrigin( );
		bDucking = pEnt->GetFlags( ) &FL_DUCKING;
		if( bDucking ) vPlayerHead = vPlayerFoot + Vector( 0, 0, 52 );
		else vPlayerHead = vPlayerFoot + Vector( 0, 0, 72 );

		if( !WorldToScreen( vPlayerFoot, vPlayerFootScreen ) ) continue;
		if( !WorldToScreen( vPlayerHead, vPlayerHeadScreen ) ) continue;

		width = ( vPlayerFootScreen.y - vPlayerHeadScreen.y ) / 4;
		espwidth = width * 2;
		espheight = vPlayerFootScreen.y - vPlayerHeadScreen.y;

		g_pDraw->BorderBoxOutlined( vPlayerHeadScreen.x - width, vPlayerHeadScreen.y, espwidth, espheight, ESP_COLOR, Color( 0, 0, 0, 255 ) );
		g_pDraw->DrawString( true, vPlayerHeadScreen.x - ( espwidth / 2 ), vPlayerHeadScreen.y - 15, Color( 255, 255, 255, 255 ), "%s", pInfo.name );
		g_pDraw->DrawString( true, vPlayerHeadScreen.x + ( espwidth / 2 ) + 5, vPlayerHeadScreen.y + ( espheight / 2 ) - 10, Color( 255, 255, 255, 255 ), pEnt->GetWeaponName( ) );
		DrawWidth = ( espwidth / 100 ) * pEnt->GetHealth( );
		g_pDraw->RectOutlined( vPlayerHeadScreen.x - ( espwidth / 2 ), vPlayerHeadScreen.y + espheight + 4, DrawWidth, 2, pEnt->GetColor( true ), Color( 0, 0, 0, 255 ) );

		static Vector vecOut = Vector( 0.0f, 0.0f, 0.0f );

		float flPitch = pEnt->GetViewPos( );

		if( ( flPitch > 80.f ) && ( flPitch <= 90.f ) )
		{
			vecTarget.z += 3.0f;
		}

		if( !WorldToScreen( vecTarget, vecOut ) ) continue;
		g_pDraw->RectOutlined( vecOut.x, vecOut.y, 2, 2, Color( 255, 255, 255, 255 ), Color( 0, 0, 0, 255 ) );

		HitBoxESP( ( C_BasePlayer* )pEnt, 12, ESP_COLOR );
	}
}

void cESP::HitBoxESP( C_BasePlayer* pTarget, int iHitbox, Color color )
{
	matrix3x4_t		pMatrix, Matrix[ 128 ];
	Vector			vMin, vMax, vBone;
	mstudiobbox_t*	box = NULL;
	const model_t*  pModel = pTarget->GetModel( );
	studiohdr_t*	pStudioHdr = g_pModelInfo->GetStudiomodel( pModel );

	if( !pTarget->SetupBones( Matrix, 128, 0x00000100,0.0f ) ) return;
	box = pStudioHdr->pHitbox( iHitbox, 0 );
	
this->DrawHitBox(Matrix[box->bone], box->bbmax, box->bbmin, color);
}

void cESP::DrawHitBox( matrix3x4_t matrix, Vector max, Vector min, Color color )
{
	Vector points[ ] = { 
		Vector( min.x, min.y, min.z ),
		Vector( min.x, max.y, min.z ),
		Vector( max.x, max.y, min.z ),
		Vector( max.x, min.y, min.z ),
		Vector( max.x, max.y, max.z ),
		Vector( min.x, max.y, max.z ),
		Vector( min.x, min.y, max.z ),
		Vector( max.x, min.y, max.z ) };

	Vector pointsTransformed[ 8 ];

	for( int i = 0; i < 8; i++ )
	{
		VectorTransform( points[ i ], matrix, pointsTransformed[ i ] );
	}

	g_pDraw->DrawBox( pointsTransformed, color );
}

bool cESP::GetHitboxPosition( int nIndex, cEntity *pEnt, Vector &vOut )
{
	matrix3x4_t		pMatrix;
	Vector			vMin, vMax, Bone;
	mstudiobbox_t*	box = NULL;
	const model_t*	pModel = pEnt->GetModel( );
	studiohdr_t*	pStudioHdr = g_pModelInfo->GetStudiomodel( pModel );

	box = pStudioHdr->pHitbox( nIndex, 0 );
	if( !GetBonePosition( ( C_BasePlayer* )pEnt, Bone, box->bone ) ) return false;
	DWORD dwBoneMatrix = *( DWORD* )( pEnt + cBoneMatrix );
	pMatrix = *(matrix3x4_t*)(dwBoneMatrix + 0xf4 * box->bone); //наверно m_nForceBone: 0x560 щас было 0x11C

	VectorTransform( box->bbmin, pMatrix, vMin );
	VectorTransform( box->bbmax, pMatrix, vMax );
	vOut = ( vMin + vMax ) * 0.5f;

	return true;
}
