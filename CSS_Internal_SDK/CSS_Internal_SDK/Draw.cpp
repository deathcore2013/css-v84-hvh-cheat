#include "Draw.h"

cDraw *g_pDraw = new cDraw( );
float DotProduct2(const float *v1, const float *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

VOID VectorTransform3(const Vector in1, const matrix3x4_t& in2, Vector& out)
{
	float buf[3];
	buf[0] = in1.x;
	buf[1] = in1.y;
	buf[2] = in1.z;

	out[0] = DotProduct2(buf, in2[0]) + in2[0][3];
	out[1] = DotProduct2(buf, in2[1]) + in2[1][3];
	out[2] = DotProduct2(buf, in2[2]) + in2[2][3];
}
void cDraw::Initialize( )
{
	if( g_pSurface == NULL ) return;

	do
	{
		g_pEngine->GetScreenSize( x, y );
	}while( ( x == 0 || y == 0 ) || ( x == 0 && y == 0 ) );

	m_ESPFont = g_pSurface->CreateFont( );
	g_pSurface->SetFontGlyphSet( m_ESPFont, "Arial", 15, 500, 0, 0, 0x200 );

	m_HUDFont = g_pSurface->CreateFont( );
	g_pSurface->SetFontGlyphSet( m_HUDFont, "Arial", 13, 500, 0, 0, 0x200 );
}

void cDraw::DrawString( bool HUDFont, int x, int y, Color dwColor, const wchar_t *pszText )
{
	if( pszText == NULL ) return;

	g_pSurface->DrawSetTextPos( x, y );
	g_pSurface->DrawSetTextFont( HUDFont ? m_HUDFont: m_ESPFont );
	g_pSurface->DrawSetTextColor( dwColor );
	g_pSurface->DrawPrintText( pszText, wcslen( pszText ) );
}

void cDraw::DrawString( bool HUDFont, int x, int y, Color dwColor, const char *pszText, ... )
{
	if( pszText == NULL ) return;

	va_list va_alist;
	char szBuffer[ 1024 ] = { '\0' };
	wchar_t szString[ 1024 ] = { '\0' };

	va_start( va_alist, pszText );
	vsprintf_s( szBuffer, pszText, va_alist );
	va_end( va_alist );

	wsprintfW( szString, L"%S", szBuffer );

	g_pSurface->DrawSetTextPos( x, y );
	g_pSurface->DrawSetTextFont( HUDFont ? m_HUDFont: m_ESPFont  );
	g_pSurface->DrawSetTextColor( dwColor );
	g_pSurface->DrawPrintText( szString, wcslen( szString ) );
}

void cDraw::DrawRect( int x, int y, int w, int h, Color dwColor )
{
	g_pSurface->DrawSetColor( dwColor );
	g_pSurface->DrawFilledRect( x, y, x + w, y + h );
}

void cDraw::DrawRectOutlined( int x, int y, int w, int h, Color dwColor, Color Outline )
{
	this->DrawRect( x, y, w, h, dwColor );
	g_pSurface->DrawSetColor( Outline );
	g_pSurface->DrawOutlinedRect( x - 1, y - 1, x + w + 1, y + h + 1 );
}

void cDraw::Rect(int x, int y, int w, int h, Color color)
{
	g_pSurface->DrawSetColor( color );
	g_pSurface->DrawOutlinedRect( x, y, x + w, y + h );
}

void cDraw::RectOutlined( int x, int y, int w, int h, Color dwColor, Color Outline )
{
	g_pSurface->DrawSetColor( Outline );
	g_pSurface->DrawOutlinedRect( x + 1, y + 1, x + w - 1, y + h - 1 );
	g_pSurface->DrawOutlinedRect( x - 1, y - 1, x + w + 1, y + h + 1 );
	this->Rect( x, y, w, h, dwColor );
}

void cDraw::BorderBox( int x, int y, int w, int h, Color dwColor )
{
	g_pSurface->DrawSetColor( dwColor );
	g_pSurface->DrawOutlinedRect( x, y, x + w, y + h );
}

void cDraw::BorderBoxOutlined( int x, int y, int w, int h, Color dwColor, Color Outline )
{
	this->BorderBox( x, y, w, h, dwColor );
	g_pSurface->DrawSetColor( Outline );
	g_pSurface->DrawOutlinedRect( x + 1, y + 1, x + w - 1, y + h - 1 );
	g_pSurface->DrawOutlinedRect( x - 1, y - 1, x + w + 1, y + h + 1 );
}

void cDraw::DrawBox3D( Vector head, Vector bottom, Vector angles, Color color ) 
{
	float theta1 = 2.0f * M_PI * ( angles.y + 45.0f ) / 360.0f;
	Vector vRotate1    = Vector( bottom.x + ( 24.0f * cosf( theta1 ) ), bottom.y + ( 24.0f * sinf( theta1 ) ), 0.0f );
	Vector vBot1       = Vector( vRotate1.x, vRotate1.y, bottom.z );
	Vector vTop1       = Vector( vRotate1.x, vRotate1.y, head.z );
	Vector _vBot1, _vTop1;
	float theta2 = 2.0f * M_PI * ( angles.y - 45.0f ) / 360.0f;
	Vector vRotate2    = Vector( bottom.x + ( 24.0f * cosf( theta2 ) ), bottom.y + ( 24.0f * sinf( theta2 ) ), 0.0f );
	Vector vBot2       = Vector( vRotate2.x, vRotate2.y, bottom.z );
	Vector vTop2       = Vector( vRotate2.x, vRotate2.y, head.z );
	Vector _vBot2, _vTop2;
	float theta3 = 2.0f * M_PI * ( angles.y + 135.0f ) / 360.0f;
	Vector vRotate3    = Vector( bottom.x + (24.0f * cosf( theta3 ) ), bottom.y + ( 24.0f * sinf( theta3 ) ), 0.0f );
	Vector vBot3       = Vector( vRotate3.x, vRotate3.y, bottom.z );
	Vector vTop3       = Vector( vRotate3.x, vRotate3.y, head.z );
	Vector _vBot3, _vTop3;
	float theta4 = 2.0f * M_PI * ( angles.y - 135.0f ) / 360.0f;
	Vector vRotate4    = Vector( bottom.x + (24.0f * cosf( theta4 ) ), bottom.y + ( 24.0f * sinf( theta4 ) ), 0.0f );
	Vector vBot4       = Vector( vRotate4.x, vRotate4.y, bottom.z );
	Vector vTop4       = Vector( vRotate4.x, vRotate4.y, head.z );
	Vector _vBot4, _vTop4;

	g_pSurface->DrawSetColor( color );

	if( WorldToScreen( vBot1, _vBot1 ) && WorldToScreen( vTop1, _vTop1 ) &&
		WorldToScreen( vBot2, _vBot2 ) && WorldToScreen( vTop2, _vTop2 ) &&
		WorldToScreen( vBot3, _vBot3 ) && WorldToScreen( vTop3, _vTop3 ) &&
		WorldToScreen( vBot4, _vBot4 ) && WorldToScreen( vTop4, _vTop4 ) )
	{
		g_pSurface->DrawLine( _vTop1.x, _vTop1.y, _vBot1.x, _vBot1.y );
		g_pSurface->DrawLine( _vTop1.x, _vTop1.y, _vBot1.x, _vBot1.y );
		g_pSurface->DrawLine( _vTop2.x, _vTop2.y, _vBot2.x, _vBot2.y );
		g_pSurface->DrawLine( _vTop3.x, _vTop3.y, _vBot3.x, _vBot3.y );
		g_pSurface->DrawLine( _vTop4.x, _vTop4.y, _vBot4.x, _vBot4.y );
		g_pSurface->DrawLine( _vTop1.x, _vTop1.y, _vTop2.x, _vTop2.y );
		g_pSurface->DrawLine( _vTop1.x, _vTop1.y, _vTop3.x, _vTop3.y );
		g_pSurface->DrawLine( _vTop4.x, _vTop4.y, _vTop2.x, _vTop2.y );
		g_pSurface->DrawLine( _vTop4.x, _vTop4.y, _vTop3.x, _vTop3.y );
		g_pSurface->DrawLine( _vBot1.x, _vBot1.y, _vBot2.x, _vBot2.y );
		g_pSurface->DrawLine( _vBot1.x, _vBot1.y, _vBot3.x, _vBot3.y );
		g_pSurface->DrawLine( _vBot4.x, _vBot4.y, _vBot2.x, _vBot2.y );
		g_pSurface->DrawLine( _vBot4.x, _vBot4.y, _vBot3.x, _vBot3.y );
	}
}

bool cDraw::Console( int x, int y, int w, int h, char *title )
{
	int	Pos[ 4 ];
	int	Size[ 3 ];
	int	Coords[ 2 ];
	float Points[ 2 ];

	Pos[ 0 ] = x;
	Pos[ 1 ] = y;
	Pos[ 2 ] = Pos[ 0 ] + 5;
	Pos[ 3 ] = Pos[ 1 ] + 20;

	Coords[ 0 ]	= Pos[ 2 ];
	Coords[ 1 ]	= Pos[ 3 ];

	Points[ 0 ]	= Coords[ 0 ];
	Points[ 1 ]	= Coords[ 1 ];

	Size[ 0 ] = 190;
	Size[ 1 ] = 205;
	Size[ 2 ] = 180;

	this->DrawRectOutlined( Pos[ 0 ], Pos[ 1 ], Size[ 0 ] + 400 + w, Size[ 1 ] + 400 + h, Color( 15, 15, 15, 200 ), Color( 0, 0, 0, 255 ) );
	this->DrawRectOutlined( Pos[ 2 ], Pos[ 3 ], Size[ 2 ] + 399 + w, Size[ 2 ] + 375 + h, Color( 45, 45, 48, 200 ), Color( 0, 0, 0, 255 ) );
	this->DrawRectOutlined( Pos[ 2 ], Pos[ 3 ] + 560 + h, 579 + w, 20, Color( 45, 45, 48, 200 ), Color( 0, 0, 0, 255 ) );

	this->DrawString( true, Pos[ 0 ] + 5, Pos[ 1 ] + 3, Color( 255, 255, 255, 255 ), "Console" );
	this->DrawString( true, Pos[ 0 ] + 8 , Pos[ 1 ] + 584 + h, Color( 255, 255, 255, 255 ), ">" );

	return true;
}

void cDraw::DrawHitBox( matrix3x4_t matrix, Vector max, Vector min, Color color )
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
		VectorTransform3(points[i], matrix, pointsTransformed[i]);
	}

	this->DrawBox( pointsTransformed, color );
}

void cDraw::DrawBox( Vector* points, Color color )
{
	Vector startPos, endPos;

	g_pSurface->DrawSetColor( color );

	for( int i = 0; i < 3; i++ )
	{
		if( WorldToScreen( points[ i ], startPos ) && WorldToScreen( points[ i + 1 ], endPos ) )
			g_pSurface->DrawLine( startPos.x, startPos.y, endPos.x, endPos.y );
	}

	if( WorldToScreen( points[ 0 ], startPos ) && WorldToScreen( points[ 3 ], endPos ) )
		g_pSurface->DrawLine( startPos.x, startPos.y, endPos.x, endPos.y );

	for( int i = 4; i < 7; i++ )
	{
		if( WorldToScreen( points[ i ], startPos ) && WorldToScreen( points[ i + 1 ], endPos ) )
			g_pSurface->DrawLine( startPos.x, startPos.y, endPos.x, endPos.y );
	}

	if( WorldToScreen( points[ 4 ], startPos ) && WorldToScreen( points[ 7 ], endPos ) )
		g_pSurface->DrawLine( startPos.x, startPos.y, endPos.x, endPos.y );

	if( WorldToScreen( points[ 0 ], startPos ) && WorldToScreen( points[ 6 ], endPos ) )
		g_pSurface->DrawLine( startPos.x, startPos.y, endPos.x, endPos.y );

	if( WorldToScreen( points[ 1 ], startPos) && WorldToScreen( points[ 5 ], endPos ) )
		g_pSurface->DrawLine( startPos.x, startPos.y, endPos.x, endPos.y );

	if( WorldToScreen( points[ 2 ], startPos ) && WorldToScreen( points[ 4 ], endPos ) )
		g_pSurface->DrawLine( startPos.x, startPos.y, endPos.x, endPos.y );

	if( WorldToScreen( points[ 3 ], startPos) && WorldToScreen( points[ 7 ], endPos ) )
		g_pSurface->DrawLine( startPos.x, startPos.y, endPos.x, endPos.y );
}

void cDraw::PlayerList( int xpos, int ypos, int w, int h )
{
	static int height = 0;
	int	Pos[ 4 ];
	int	Size[ 3 ];
	int	Coords[ 2 ];
	float Points[ 2 ];

	Pos[ 0 ] = xpos;
	Pos[ 1 ] = ypos;
	Pos[ 2 ] = Pos[ 0 ] + 5;
	Pos[ 3 ] = Pos[ 1 ] + 20;

	Coords[ 0 ]	= Pos[ 2 ];
	Coords[ 1 ]	= Pos[ 3 ];

	Points[ 0 ]	= Coords[ 0 ];
	Points[ 1 ]	= Coords[ 1 ];

	Size[ 0 ] = 190;
	Size[ 1 ] = 205;
	Size[ 2 ] = 180;

	this->DrawRectOutlined( Pos[ 0 ], Pos[ 1 ], Size[ 0 ] + 75 + w, Size[ 1 ] - 165 + h + height, Color( 15, 15, 15, 200 ), Color( 0, 0, 0, 255 ) );
	this->DrawRectOutlined( Pos[ 2 ], Pos[ 3 ], Size[ 2 ] + 74 + w, Size[ 2 ] - 165 + h + height, Color( 45, 45, 48, 200 ), Color( 0, 0, 0, 255 ) );

	this->DrawString( true, Pos[ 0 ] + 5, Pos[ 1 ] + 3, Color( 255, 255, 255, 255 ), "Playerlist" );
	this->DrawString( true, Pos[ 0 ] + 8, Pos[ 1 ] + 21, Color( 255, 255, 255, 255 ), "ID" );
	this->DrawString( true, Pos[ 0 ] + 33, Pos[ 1 ] + 21, Color( 255, 255, 255, 255 ), "Name" );
	this->DrawString( true, Pos[ 0 ] + 115, Pos[ 1 ] + 21, Color( 255, 255, 255, 255 ), "Pitch" );
	this->DrawString( true, Pos[ 0 ] + 155, Pos[ 1 ] + 21, Color( 255, 255, 255, 255 ), "Yaw" );
	this->DrawString( true, Pos[ 0 ] + 195, Pos[ 1 ] + 21, Color( 255, 255, 255, 255 ), "Pitch Fix" );
	this->DrawString( true, Pos[ 0 ] + 265, Pos[ 1 ] + 21, Color( 255, 255, 255, 255 ), "Body Aim" );
	this->DrawString( true, Pos[ 0 ] + 325, Pos[ 1 ] + 21, Color( 255, 255, 255, 255 ), "Friend" );

	g_pSurface->DrawSetColor( Color( 0, 0, 0, 255 ) );
	g_pSurface->DrawLine( Pos[ 0 ] + 28, Pos[ 1 ] + 20, Pos[ 0 ] + 28, Pos[ 1 ] + 20 + height + 15 );
	g_pSurface->DrawLine( Pos[ 0 ] + 110, Pos[ 1 ] + 20, Pos[ 0 ] + 110, Pos[ 1 ] + 20 + height + 15 );
	g_pSurface->DrawLine( Pos[ 0 ] + 150, Pos[ 1 ] + 20, Pos[ 0 ] + 150, Pos[ 1 ] + 20 + height + 15 );
	g_pSurface->DrawLine( Pos[ 0 ] + 190, Pos[ 1 ] + 20, Pos[ 0 ] + 190, Pos[ 1 ] + 20 + height + 15 );
	g_pSurface->DrawLine( Pos[ 0 ] + 260, Pos[ 1 ] + 20, Pos[ 0 ] + 260, Pos[ 1 ] + 20 + height + 15 );
	g_pSurface->DrawLine( Pos[ 0 ] + 320, Pos[ 1 ] + 20, Pos[ 0 ] + 320, Pos[ 1 ] + 20 + height + 15 );
	g_pSurface->DrawLine( Pos[ 0 ] + 4, Pos[ 1 ] + 35, Pos[ 0 ] + 259 + w, Pos[ 1 ] + 35 );

	cEntity *pLocal = ( cEntity* )g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );
	if( !pLocal ) return;

	player_info_t pInfo;

	int iHeight = 0;
	height = 0;

	char *szFriend = "false";
	char *szMagic = "down";
	char *szBodyAim = "false";

	for( int i = 1; i <= pGlobals->maxClients; i++ )
	{
		cEntity *pBaseEnt = ( cEntity* )g_pEntList->GetClientEntity( i );
		if( pBaseEnt == pLocal || !pBaseEnt ) continue;
		if( !g_pEngine->GetPlayerInfo( i, &pInfo ) ) continue;
		iHeight += 15;
		height = iHeight;
		szFriend = "false";
		szMagic = "down";
		szBodyAim = "false";
		this->DrawString( true, Pos[ 0 ] + 8 , Pos[ 1 ] + 20 + iHeight + 2, Color( 255, 255, 255, 255 ), "%i", i );
		this->DrawString( true, Pos[ 0 ] + 33 , Pos[ 1 ] + 20 + iHeight + 2, Color( 255, 255, 255, 255 ), "%s", pInfo.name );
		this->DrawString( true, Pos[ 0 ] + 115, Pos[ 1 ] + 20 + iHeight + 2, Color( 255, 255, 255, 255 ), "%s", "auto" );
		this->DrawString( true, Pos[ 0 ] + 155, Pos[ 1 ] + 20 + iHeight + 2, Color( 255, 255, 255, 255 ), "%s", "auto" );
		if( sPlayerList[ i ].bMagicPitch ) szMagic = "up";
		this->DrawString( true, Pos[ 0 ] + 195, Pos[ 1 ] + 20 + iHeight + 2, Color( 255, 255, 255, 255 ), "%s", szMagic );
		if( sPlayerList[ i ].bBodyAim ) szBodyAim = "true";
		this->DrawString( true, Pos[ 0 ] + 265, Pos[ 1 ] + 20 + iHeight + 2, Color( 255, 255, 255, 255 ), "%s", szBodyAim );
		if( sPlayerList[ i ].bIsFriend ) szFriend = "true";
		this->DrawString( true, Pos[ 0 ] + 325, Pos[ 1 ] + 20 + iHeight + 2, Color( 255, 255, 255, 255 ), "%s", szFriend );
		g_pSurface->DrawLine( Pos[ 0 ] + 4, Pos[ 1 ] + 35 + iHeight, Pos[ 0 ] + 259 + w, Pos[ 1 ] + 35 + iHeight );
	}
}

void cDraw::CrossHair( int iSize )
{
	int xWindow, yWindow;
	g_pEngine->GetScreenSize( xWindow, yWindow );

	xWindow /= 2;
	yWindow /= 2;

	this->Rect( xWindow - iSize, yWindow, iSize * 2 + 1, 1, Color( 255, 0, 0, 255 ) );
	this->Rect( xWindow, yWindow - iSize, 1, iSize * 2 + 1, Color( 255, 0, 0, 255 ) );
	this->Rect( xWindow - iSize, yWindow - iSize, 1, iSize, Color( 255, 0, 0, 255 ) );
	this->Rect( xWindow, yWindow - iSize, iSize + 1, 1, Color( 255, 0, 0, 255 ) );
	this->Rect( xWindow + iSize + 1, yWindow, 1, iSize + 1, Color( 255, 0, 0, 255 ) );
	this->Rect( xWindow - iSize, yWindow + iSize, iSize + 1, 1, Color( 255, 0, 0, 255 ) );

	g_pCvar->FindVar( "crosshair" )->SetValue( "0" );
}