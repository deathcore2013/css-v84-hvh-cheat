#ifndef CDRAW_H
#define CDRAW_H

#include "stdafx.h"
#include "Math.h"
#include "Globals.h"
#include "Stuff.h"

class cDraw
{
public:
	void Initialize( );
	void DrawString( bool HUDFont, int x, int y, Color dwColor, const wchar_t *pszText);
	void DrawString( bool HUDFont, int x, int y, Color dwColor, const char *pszText, ... );
	void DrawRect( int x, int y, int w, int h, Color dwColor );
	void DrawRectOutlined( int x, int y, int w, int h, Color dwColor, Color Outline );
	void BorderBox( int x, int y, int w, int h, Color dwColor );
	void BorderBoxOutlined( int x, int y, int w, int h, Color dwColor, Color Outline );
	void DrawBox3D( Vector head, Vector bottom, Vector angles, Color color );
	void Rect( int x, int y, int w, int h, Color color );
	void RectOutlined( int x, int y, int w, int h, Color dwColor, Color Outline );
	bool Console( int x, int y, int w, int h, char *title );
	void DrawHitBox( matrix3x4_t matrix, Vector max, Vector min, Color color );
	void DrawBox( Vector* points, Color color );
	void PlayerList( int xpos, int ypos, int w, int h );
	void CrossHair( int iSize );

	unsigned long m_ESPFont;
	unsigned long m_HUDFont;
};

extern cDraw *g_pDraw;

#endif