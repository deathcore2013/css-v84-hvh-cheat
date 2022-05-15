#include "Tools.h"

bool Compare( const BYTE* pData, unsigned char* bMask, const char* szMask )
{
	for( ; *szMask; ++szMask, ++pData, ++bMask) 
		if( *szMask == 'x' && *pData != *bMask )   return 0;
	return ( *szMask ) == NULL;
}

DWORD FindPattern( DWORD dwAddress, DWORD dwLen, unsigned char *bMask, char * szMask, DWORD Offset = 0x1 )
{
	for( DWORD i = 0; i < dwLen; i++ )
		if( Compare( ( BYTE* )( dwAddress + i ), bMask, szMask ) )  return ( DWORD )( dwAddress + i + Offset );
	return 0;
}
