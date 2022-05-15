#ifndef _CCONSOLE_H_
#define _CCONSOLE_H_

#include "stdafx.h"
#include <vector>

using namespace std;

class cConsole
{
public:
	void Init( );
	void Clear( );
	void Draw( );
	void TextToConsole( const string &str, int x, int y );
	void echo( const char*,... );
	int GetWidth( const char *input );
	void CommandHandler( string cmd );
	string CheckComp( string tempInput );
	bool active;
	string cmdLine;
	int lineIndex;
	int scrollCount;
	int tick;
	bool cursor;
	char *cursorpos;
	vector<string>conLines;
	vector<string>prevLines;
	HFONT hfont;
	int xpos;
	int ypos;
	int wpos;
	int hpos;
	vector<string> dict;
	bool completed;
	string iCvar;
};

extern cConsole *g_pConsole;

#endif