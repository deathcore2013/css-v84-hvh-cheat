#include "cEngine.h"

Engine *pEngine = new Engine();

int Engine::GetWeaponType( int weaponid )
{
	if( weaponid == WEAPON_GLOCK || weaponid == WEAPON_USP45 || weaponid == WEAPON_FIVE7
		|| weaponid == WEAPON_ELITE || weaponid == WEAPON_P228 || weaponid == WEAPON_DEAGLE )
		return WEAPON_PISTOL;
	if(weaponid == WEAPON_M3SUPER || weaponid == WEAPON_XM1014) return WEAPON_SHOTGUN;
	else if( weaponid == WEAPON_MAC10 || weaponid == WEAPON_UMP45 || weaponid == WEAPON_MP5 
		|| weaponid == WEAPON_P90 ) 
		return WEAPON_MP;
	else if( weaponid == WEAPON_M4A1 || weaponid == WEAPON_AK47 || weaponid == WEAPON_GALIL	
		|| weaponid == WEAPON_FAMAS || weaponid == WEAPON_AUG || weaponid == WEAPON_SG552 )
		return WEAPON_RIFLE;
	else if( weaponid == WEAPON_AWP || weaponid == WEAPON_SCOUT || weaponid == WEAPON_G3SG1
		|| weaponid == WEAPON_SG550 )
		return WEAPON_SNIPER;
	else if( weaponid == WEAPON_M249 ) return WEAPON_MG;
	else if( weaponid == WEAPON_KNIFE || weaponid == WEAPON_HE || weaponid == WEAPON_FLASH 
		|| weaponid == WEAPON_SMOKE || weaponid == WEAPON_C4 ) return 0;
	else return -1;
}

bool Engine::IsAutomatic( int weaponid )
{
	if( weaponid == WEAPON_GLOCK || weaponid == WEAPON_USP45 || weaponid == WEAPON_FIVE7
		 || weaponid == WEAPON_P228 || weaponid == WEAPON_DEAGLE  || weaponid == WEAPON_AWP 
		 || weaponid == WEAPON_SCOUT ) 
		 return false;
	return true;
}