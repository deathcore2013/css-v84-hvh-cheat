#include "Console.h"
#include "Draw.h"

cConsole *g_pConsole = new cConsole( );

string cvars[ ] = { "clear", "set_console_x", "set_console_y", "set_console_w", "set_console_h", "playerlist_friend", "playerlist_clear", "playerlist_magic_pitch",
	                "playerlist_body_aim", "fakelag_cmd", "antiaim_type" };

void cConsole::Init( )
{
	for( int i = 0; i < 11; i++ )
	{
		dict.push_back( cvars[ i ] );
	}

	active = false;
	lineIndex = 0;
	cmdLine.clear( );
	conLines.push_back( "" );
	cursorpos = (char*)cmdLine.c_str( );

	xpos = 600;
	ypos = 5;
	wpos = 0;
	hpos = 0;
	hfont = CreateFontA( 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Arial" );
	completed = false;
}

int cConsole::GetWidth( const char *input )
{
	HDC dc = GetDC( NULL );
	SelectObject( dc, hfont );

	RECT rect = { 0, 0, 0, 0 };
	DrawText( dc, input, strlen( input ), &rect, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE );
	int textWidth = rect.right;

	DeleteDC( dc );
	return textWidth;
}

void cConsole::Draw( )
{
	if( !active ) 
	{
		cmdLine.clear( );
		return;	
	}

	int iTextPos = ypos + 576 + hpos;

	tick += 1;
	if ( tick >= 120 )
	{
		tick = 0;
		cursor = !cursor;
	}

	if( !g_pDraw->Console( xpos, ypos, wpos, hpos, "Console" ) ) return;

	g_pDraw->DrawString( true, xpos + 18, ypos + 584 + hpos, Color( 255, 255, 255, 255 ), cmdLine.c_str( ) );

	if( cursor )
	{
		int length = this->GetWidth( ( const char * )cmdLine.c_str( ) );
		char save = cursorpos[ 0 ];
		cursorpos[ 0 ] = save;
		save = cursorpos[ 1 ];
		cursorpos[ 1 ] = 0;
		cursorpos[ 1 ] = save;
		g_pDraw->DrawString( true, xpos + 18 + length, ypos + 584 + hpos, Color( 255, 255, 255, 255 ), "_" );

		if( this->GetWidth( ( const char* )cmdLine.c_str( ) ) > 450 )
		{
			cmdLine.clear( );
			echo( "Console has been cleared because the word you typed in was to long" );
		}
	}

	iCvar = CheckComp( cmdLine );
	if( completed )
	{
		g_pDraw->DrawRectOutlined( xpos + 8, ypos + 608 + hpos, GetWidth( iCvar.c_str( ) ) + 7, 15, Color( 45, 45, 48, 200 ), Color( 0, 0, 0, 255 ) );
		g_pDraw->DrawString( true, xpos + 11, ypos + 609 + hpos, Color( 255, 255, 255, 255 ), iCvar.c_str( ) );
	}

	if( !conLines.size( ) ) return;

	for( int i = conLines.size( ) - 1 - scrollCount; i > 0; i-- )
	{
		string &strLine = conLines[ i ];
		TextToConsole( strLine, xpos - 1, iTextPos -= 15 );
		if( iTextPos - 15 <= ( ypos + 16 ) ) break;
	}
}

void cConsole::Clear( )
{
	cmdLine.clear( );
	conLines.clear( );
	conLines.push_back( "" );
	scrollCount = 0;
}

void cConsole::echo( const char *fmt,... )
{
	char buf[ 260 ];
	vsprintf( buf, fmt, ( char* )&fmt + _INTSIZEOF( fmt ) );
	string strLine( buf );
	conLines.push_back( strLine );
}

void cConsole::TextToConsole( const string &str, int x, int y )
{
	const char *line = str.c_str( );
	char buf[ 256 ], *bufpos;
	for( ;; )
	{
		bufpos = buf;
		for( ;; )
		{
			*bufpos = *line;
			if( !*line || *line == '^' )
				break;
			++line;
			++bufpos;
		}
		bufpos[ 0 ] = 0;
		bufpos[ 1 ] = 0;

		g_pDraw->DrawString( true, x + 8, y, Color( 255, 255, 255, 255 ), buf );
		break;
	}
}

string cConsole::CheckComp( string tempInput )
{
	int count = 0;
	int line;

	for( int i = 0; i < dict.size( ); i++ )
	{
		if( dict[ i ].find( tempInput.c_str( ) ) != string::npos )
		{
			count++;
			line = i;
		}
	}

	if( count == 1 )
	{
		completed = true;
		return dict[ line ];
	}

	completed = false;
	return "";
}

void SplitText( string str, string &out1, string &out2 )
{
	int found = str.find_last_of( "," );
	out1 = str.substr( 0, found );
	out2 = str.substr( found + 1 );
}

void cConsole::CommandHandler( string cmd )
{
	string output = "] " + cmd;
	echo( output.c_str( ) );
	output.clear( );

	string key, value;
	int equal_pos = 0;

	for( int i = 0; i < cmd.length( ); i++ )
	{
		if( cmd[ i ] == ' ' )
		{
			equal_pos = i;
		}
		key = cmd.substr( 0, equal_pos );
		value = cmd.substr( equal_pos + 1, cmd.length( ) );
	}

	output = "Unknown command \"" + cmd + "\"";

	if( key == "set_console_x" )
	{
		xpos = atoi( value.c_str( ) );
		output = key + " has been set to " + value;
	}

	if( key == "set_console_y" )
	{
		ypos = atoi( value.c_str( ) );
		output = key + " has been set to " + value;
	}

	if( key == "set_console_w" )
	{
		wpos = atoi( value.c_str( ) );
		output = key + " has been set to " + value;
	}

	if( key == "set_console_h" )
	{
		hpos = atoi( value.c_str( ) );
		output = key + " has been set to " + value;
	}

	if( cmd == "clear" )
	{
		this->Clear( );
		output = "Console has been cleared";
	}

	if( key == "playerlist_friend" )
	{
		string szPlayerID = "";
		string szValue = "";
		SplitText( value, szPlayerID, szValue );
		sPlayerList[ atoi( szPlayerID.c_str( ) ) ].bIsFriend = atoi( szValue.c_str( ) );
		player_info_t pInfo;
		if( !g_pEngine->GetPlayerInfo( atoi( szPlayerID.c_str( ) ), &pInfo ) ) return;
		if( sPlayerList[ atoi( szPlayerID.c_str( ) ) ].bIsFriend )
		{
			output = ( string )pInfo.name + " is now marked as your friend";
		}
		else
		{
			output = ( string )pInfo.name + " is now marked as your enemy";
		}
	}

	if( key == "playerlist_body_aim" )
	{
		string szPlayerID = "";
		string szValue = "";
		SplitText( value, szPlayerID, szValue );
		sPlayerList[ atoi( szPlayerID.c_str( ) ) ].bBodyAim = atoi( szValue.c_str( ) );
		player_info_t pInfo;
		if( !g_pEngine->GetPlayerInfo( atoi( szPlayerID.c_str( ) ), &pInfo ) ) return;
		if( sPlayerList[ atoi( szPlayerID.c_str( ) ) ].bBodyAim )
		{
			output = "Body aim has been activated at " + ( string )pInfo.name;
		}
		else
		{
			output = "Body aim has been deactivated at " + ( string )pInfo.name;
		}
	}

	if( key == "playerlist_magic_pitch" )
	{
		string szPlayerID = "";
		string szValue = "";
		SplitText( value, szPlayerID, szValue );
		sPlayerList[ atoi( szPlayerID.c_str( ) ) ].bMagicPitch = atoi( szValue.c_str( ) );
		player_info_t pInfo;
		if( !g_pEngine->GetPlayerInfo( atoi( szPlayerID.c_str( ) ), &pInfo ) ) return;
		output = "Magic Pitch has been set to " + szValue + " at " + ( string )pInfo.name;
	}

	if( cmd == "playerlist_clear" )
	{
		ResetPlayerList( );
		output = "Playerlist has been reseted";
	}

	if( key == "fakelag_cmd" )
	{
		g_iFakeLaggEnd = atoi( value.c_str( ) );
		output = key + " has been set to " + value;
	}

	if( key == "antiaim_type" )
	{
		g_iAntiAimType = atoi( value.c_str( ) );
		output = key + " has been set to " + value;
	}

	echo( output.c_str( ) );
	cmd.clear( );
	output.clear( );
}


