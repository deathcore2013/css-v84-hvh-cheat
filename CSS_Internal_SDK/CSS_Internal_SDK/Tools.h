#ifndef TOOLS_H
#define TOOLS_H

#include <Windows.h>

extern DWORD FindPattern( DWORD dwAddress, DWORD dwLen, unsigned char *bMask, char * szMask, DWORD Offset );

#endif
