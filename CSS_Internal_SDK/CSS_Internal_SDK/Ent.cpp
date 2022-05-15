#include "Ent.h"

bool cEntity::IsValid( void )
{
	return ( this && this->IsDormant( ) == false && this->GetHealth( ) > 0 &&  this->GetHealth( ) <= 100 && this->GetLifestate( ) == LIFE_ALIVE );
}

bool cEntity::IsAlive( void )
{
	return( this->GetHealth( ) > 0 && this->GetLifestate( ) == LIFE_ALIVE );
}

Vector cEntity::GetAbsOrigin( void )
{
	return ( *( Vector * )( this + c_vecOrigin ) );
}

Vector cEntity::GetNormalAbsOrigin( void )
{
	return(*(Vector*)(this + 0xE8)); //или 0x25C хз
}

Vector cEntity::GetEyePosition( void )
{
	return ( *( Vector * )( this + c_vecOrigin ) + *( Vector * )( this + c_viewOrigin ) );
}

Vector cEntity::GetNormalEyePosition( void )
{
	return ( *( Vector * )( this + 0xE8 ) + *( Vector * )( this + c_viewOrigin ) ); // или это хз 0x25C
}

BOOL cEntity::IsDormant( void )
{
	return ( *( bool* ) ( this + c_bIsDormant ) );
}

BYTE cEntity::GetFlags( void )
{
	return ( *( BYTE * )( this + c_fFlags ) );
}

BYTE cEntity::GetLifestate( void )
{
	return ( *( BYTE * )( this + c_lifeState ) );
}

int cEntity::GetHealth( void )
{
	return ( *( int * )( this + c_iHealth ) );
}

int cEntity::GetTeam( void )
{
	return ( *( int* )( this + c_iTeamNum ) );
}

Vector cEntity::GetBonePosition( int iBone )
{
	DWORD dwBoneMatrix = *( DWORD* )( this + cBoneMatrix );
	Vector BoneOrigin;
	BoneOrigin.x = *(float*)(dwBoneMatrix + 0x30 * iBone + 0x0C);
	BoneOrigin.y = *(float*)(dwBoneMatrix + 0x30 * iBone + 0x1C);
	BoneOrigin.z = *(float*)(dwBoneMatrix + 0x30 * iBone + 0x2C);
	return BoneOrigin;
}

int cEntity::GetFOV( void )
{
	int iFOV = *( int* )( this + c_iFOV );
	if( iFOV == 0 ) iFOV = 90;
	return iFOV;
}

cEntity *cEntity::GetCombatWeapon( void )
{
	unsigned long ulWeapon = *( unsigned long* )( this + c_hActiveWeapon );
	ulWeapon &= 0xFFF;
	cEntity *pWeapon = ( cEntity* )( cEntity* )g_pEntList->GetClientEntity( ulWeapon );
	return pWeapon;
}

int cEntity::GetAmmo( void )
{
	cEntity *pWeapon = this->GetCombatWeapon();
	return ( *( int* )( pWeapon + c_iClip1 ) );
}

model_t *cEntity::GetModel( void )
{
	__asm
	{
		MOV EDI, this
		LEA ECX, [EDI + 0x4]
		MOV EDX, DWORD PTR DS:[ECX]
		CALL [EDX + 0x24]
	}
}

int cEntity::GetWeaponID( void )
{
	cEntity *pWeapon = this->GetCombatWeapon( );
	if( !pWeapon ) return 0;
	DWORD dwWeapon = *( DWORD* )( pWeapon );
	DWORD dwTemp = *( DWORD* )( dwWeapon + 0x5A0 ); //должен быть этот 0x5B4
	return ( *( int *) ( dwTemp + 1 ) );
}

char *cEntity::GetWeaponName( void )
{
	switch( this->GetWeaponID( ) )
	{
	case WEAPON_P228:
		return "P228"; break;
	case WEAPON_GLOCK:
		return "Glock"; break;
	case WEAPON_SCOUT:
		return "Scout"; break;
	case WEAPON_HE:
		return "HE"; break;
	case WEAPON_XM1014:
		return "XM1014"; break;
	case WEAPON_C4:
		return "C4"; break;
	case WEAPON_MAC10:
		return "Mac"; break;
	case WEAPON_AUG:
		return "Aug"; break;
	case WEAPON_SMOKE:
		return "Smoke"; break;
	case WEAPON_ELITE:
		return "Elite"; break;
	case WEAPON_FIVE7:
		return "Five 7"; break;
	case WEAPON_UMP45:
		return "UMP45"; break;
	case WEAPON_SG550:
		return "SG550"; break;
	case WEAPON_GALIL:
		return "Galil"; break;
	case WEAPON_FAMAS:
		return "Famas"; break;
	case WEAPON_USP45:
		return "Usp"; break;
	case WEAPON_AWP:
		return "AWP"; break;
	case WEAPON_MP5:
		return "MP5"; break;
	case WEAPON_M249:
		return "M249"; break;
	case WEAPON_M3SUPER:
		return "M3"; break;
	case WEAPON_M4A1:
		return "M4A1"; break;
	case WEAPON_TMP:
		return "TMP"; break;
	case WEAPON_G3SG1:
		return "G3SG1"; break;
	case WEAPON_FLASH:
		return "Flash"; break;
	case WEAPON_DEAGLE:
		return "Deagle"; break;
	case WEAPON_SG552:
		return "SG552"; break;
	case WEAPON_AK47:
		return "Ak47"; break;
	case WEAPON_KNIFE:
		return "Knife"; break;
	case WEAPON_P90:
		return "P90"; break;
	}
	return "";
}

bool cEntity::IsDefusing( void )
{
	return ( *( bool* )( this + c_bIsDefusing ) );
}

QAngle cEntity::GetPunchAngles( void )
{
	return ( *( QAngle* )( this + c_vecPunch ) );
}

int cEntity::iShots( void )
{
	return (*(int*)(this + 0x1430)); //обновлен
}

Vector cEntity::GetVelocity( void )
{
	return( *( Vector* )( this + 0xF4 ) ); //обновлен
}

bool cEntity::CanShoot( void )
{
	if( pEngine->GetWeaponType( this->GetWeaponID( ) ) > 0 )
	{
		cEntity *pWeapon = this->GetCombatWeapon( );
		if( !pWeapon )
		{
			return false;
		}
		float m_flNextPrimaryAttack = *(float*)((DWORD)pWeapon + 0x878);  //обновлен
		float m_nTickbase = *(int*)(this + 0x10ec); //обновлен
		float flTick = m_nTickbase * pGlobals->interval_per_tick;
		if( m_flNextPrimaryAttack > flTick )
		{
			return false;
		}
		return true;
	}
	return false;
}

float cEntity::flSimulationTime( void )
{
	return(*(float*)(this + 0x6c)); //обновлен
}

Color cEntity::GetColor( bool alpha )
{
	float g = this->GetHealth( ) * 2.55f;
	float r = 255 - g;
	int a = 255;
	if( !alpha ) a = 30;
	Color color = Color( ( int )r, ( int )g, 0, a );
	return color;
}

float cEntity::GetViewPos( )
{
	return(*(float*)(this + 0xb8c)); //вроде обновлен
}