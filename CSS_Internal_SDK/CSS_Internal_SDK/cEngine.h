#ifndef CENGINE_H
#define CENGINE_H

#include "stdafx.h"
#include "Globals.h"

class Engine
{
public:
	int		GetWeaponType( int weaponid );
	bool	IsAutomatic( int weaponid );
};
extern Engine *pEngine;

#endif