#include "Linker.h"

Linker *g_pLinker = new Linker( );

typedef struct _PEB_LDR_DATA {
	UINT8 _PADDING_[ 12 ];
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB {
	UINT8 _PADDING_[ 12 ];
	PEB_LDR_DATA* Ldr;
} PEB, *PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	VOID* DllBase;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _UNLINKED_MODULE
{
	HMODULE hModule;
	PLIST_ENTRY RealInLoadOrderLinks;
	PLIST_ENTRY RealInMemoryOrderLinks;
	PLIST_ENTRY RealInInitializationOrderLinks;
	PLDR_DATA_TABLE_ENTRY Entry; 
} UNLINKED_MODULE;

#define UNLINK( x )                     \
	( x ).Flink->Blink = ( x ).Blink;   \
	( x ).Blink->Flink = ( x ).Flink;

#define RELINK( x, real )           \
	( x ).Flink->Blink = (real);    \
	( x ).Blink->Flink = (real);    \
	( real )->Blink = ( x ).Blink;  \
	( real )->Flink = ( x ).Flink;

std::vector<UNLINKED_MODULE> UnlinkedModules;

struct FindModuleHandle
{
	HMODULE m_hModule;
	FindModuleHandle( HMODULE hModule ) : m_hModule( hModule )
	{
		//nothing
	}
	bool operator( ) ( UNLINKED_MODULE const &Module ) const
	{
		return ( Module.hModule == m_hModule );
	}
};

void Linker::RelinkModuleToPEB( HMODULE hModule )
{
	std::vector<UNLINKED_MODULE>::iterator it = std::find_if( UnlinkedModules.begin( ), UnlinkedModules.end( ), FindModuleHandle( hModule ) );

	if( it == UnlinkedModules.end( ) )
	{
		return;
	}

	RELINK( ( *it ).Entry->InLoadOrderLinks, ( *it ).RealInLoadOrderLinks );
	RELINK( (*it ).Entry->InInitializationOrderLinks, ( *it ).RealInInitializationOrderLinks );
	RELINK( ( *it ).Entry->InMemoryOrderLinks, ( *it ).RealInMemoryOrderLinks );
	UnlinkedModules.erase( it );
}

void Linker::UnlinkModuleFromPEB( HMODULE hModule )
{
	std::vector<UNLINKED_MODULE>::iterator it = std::find_if( UnlinkedModules.begin( ), UnlinkedModules.end( ), FindModuleHandle( hModule ) );
	if( it != UnlinkedModules.end( ) )
	{
		return;
	}

#ifdef _WIN64
	PPEB pPEB = ( PPEB )__readgsqword( 0x60 );
#else
	PPEB pPEB = ( PPEB )__readfsdword( 0x30 );
#endif

	PLIST_ENTRY CurrentEntry = pPEB->Ldr->InLoadOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY Current = NULL;

	while( CurrentEntry != &pPEB->Ldr->InLoadOrderModuleList && CurrentEntry != NULL )
	{
		Current = CONTAINING_RECORD( CurrentEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );
		if( Current->DllBase == hModule )
		{
			UNLINKED_MODULE CurrentModule = { 0 };
			CurrentModule.hModule = hModule;
			CurrentModule.RealInLoadOrderLinks = Current->InLoadOrderLinks.Blink->Flink;
			CurrentModule.RealInInitializationOrderLinks = Current->InInitializationOrderLinks.Blink->Flink;
			CurrentModule.RealInMemoryOrderLinks = Current->InMemoryOrderLinks.Blink->Flink;
			CurrentModule.Entry = Current;
			UnlinkedModules.push_back( CurrentModule );

			UNLINK( Current->InLoadOrderLinks );
			UNLINK( Current->InInitializationOrderLinks );
			UNLINK( Current->InMemoryOrderLinks );

			break;
		}
		CurrentEntry = CurrentEntry->Flink;
	}
}