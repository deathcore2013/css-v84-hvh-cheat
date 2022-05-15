#ifndef _iSurface_
#define _iSurface_

#include "stdafx.h"

class pSurface
{
public:
	void DrawSetColor(Color colorcode)
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, Color );
		CallVirtual< OriginalFn >( this, 10 )( this, colorcode );
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int, int, int, int );
		CallVirtual< OriginalFn >( this, 12 )( this, x0, y0, x1, y1 );
	}

	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int, int, int, int );
		CallVirtual< OriginalFn >( this, 14 )( this, x0, y0, x1, y1 );
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int , int , int , int  );
		CallVirtual< OriginalFn >( this, 15 )( this, x0, y0, x1, y1 );
	}

	void DrawSetTextFont(unsigned long font)
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, unsigned long );
		CallVirtual< OriginalFn >( this, 17 )( this, font );
	}

	void DrawSetTextColor(Color colorcode)
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, Color );
		CallVirtual< OriginalFn >( this, 18 )( this, colorcode );
	}

	void DrawSetTextPos(int x, int y )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, int, int );
		CallVirtual< OriginalFn >( this, 20 )( this, x, y );
	}

	void DrawPrintText(const wchar_t *text, int textLen )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, const wchar_t *, int, int );
		return CallVirtual< OriginalFn >( this, 22 )( this, text, textLen, 0 );
	}

	unsigned long CreateFont( )
	{
		typedef unsigned int ( __thiscall* OriginalFn )( PVOID );
		return CallVirtual< OriginalFn >( this, 66 )( this );
	}

	void SetFontGlyphSet (unsigned long &font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags )
	{
		typedef void ( __thiscall* OriginalFn )( PVOID, unsigned long, const char*, int, int, int, int, int, int, int );
		CallVirtual< OriginalFn >( this, 67 )( this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0 );
	}
};

#endif