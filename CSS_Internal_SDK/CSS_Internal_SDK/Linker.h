#ifndef CLINKER_H
#define CLINKER_H

#include "stdafx.h"

#include <vector>
#include <algorithm>

class Linker
{
public:
	void RelinkModuleToPEB( HMODULE hModule );
	void UnlinkModuleFromPEB( HMODULE hModule );
};

extern Linker *g_pLinker;

#endif