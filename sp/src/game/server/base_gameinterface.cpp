//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "gameinterface.h"
#include "mapentities.h"
#include "SMMOD/mapadd.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void CServerGameClients::GetPlayerLimits( int& minplayers, int& maxplayers, int &defaultMaxPlayers ) const
{
	//minplayers = defaultMaxPlayers = 1; 
	//maxplayers = MAX_PLAYERS;
	minplayers = 1;
	defaultMaxPlayers = 1;
	maxplayers = 1;
}

// -------------------------------------------------------------------------------------------- //
// Mod-specific CServerGameDLL implementation.
// -------------------------------------------------------------------------------------------- //

void CServerGameDLL::LevelInit_ParseAllEntities( const char *pMapEntities )
{
	//after that, spawn all custom entities - Bitl
	CMapAdd *pMapadd = GetMapAddEntity();
	char szMapadd[128];
	Q_snprintf(szMapadd, sizeof(szMapadd), "mapadd/%s.txt", STRING( gpGlobals->mapname ));
	if (!pMapadd)
	{
		pMapadd = CreateMapAddEntity();
		pMapadd->RunLabel(szMapadd, "Init");
	}
	else
	{
		pMapadd->RunLabel(szMapadd, "Init");
	}

	if (g_pGameRules->GetGamemode() == FIREFIGHT_PRIMARY_COMBINEFIREFIGHT)
	{
		CMapAdd* pMapadd = GetMapAddEntity();
		char szMapadd[128];
		Q_snprintf(szMapadd, sizeof(szMapadd), "mapadd/%s.txt", STRING(gpGlobals->mapname));
		if (!pMapadd)
		{
			pMapadd = CreateMapAddEntity();
			pMapadd->RunLabel(szMapadd, "CombineFirefight");
		}
		else
		{
			pMapadd->RunLabel(szMapadd, "CombineFirefight");
		}
	}
	else if (g_pGameRules->GetGamemode() == FIREFIGHT_PRIMARY_XENINVASION)
	{
		CMapAdd* pMapadd = GetMapAddEntity();
		char szMapadd[128];
		Q_snprintf(szMapadd, sizeof(szMapadd), "mapadd/%s.txt", STRING(gpGlobals->mapname));
		if (!pMapadd)
		{
			pMapadd = CreateMapAddEntity();
			pMapadd->RunLabel(szMapadd, "XenInvasion");
		}
		else
		{
			pMapadd->RunLabel(szMapadd, "XenInvasion");
		}
	}
	else if (g_pGameRules->GetGamemode() == FIREFIGHT_PRIMARY_ANTLIONASSAULT)
	{
		CMapAdd* pMapadd = GetMapAddEntity();
		char szMapadd[128];
		Q_snprintf(szMapadd, sizeof(szMapadd), "mapadd/%s.txt", STRING(gpGlobals->mapname));
		if (!pMapadd)
		{
			pMapadd = CreateMapAddEntity();
			pMapadd->RunLabel(szMapadd, "AntlionAssault");
		}
		else
		{
			pMapadd->RunLabel(szMapadd, "AntlionAssault");
		}
	}
	else if (g_pGameRules->GetGamemode() == FIREFIGHT_PRIMARY_ZOMBIESURVIVAL)
	{
		CMapAdd* pMapadd = GetMapAddEntity();
		char szMapadd[128];
		Q_snprintf(szMapadd, sizeof(szMapadd), "mapadd/%s.txt", STRING(gpGlobals->mapname));
		if (!pMapadd)
		{
			pMapadd = CreateMapAddEntity();
			pMapadd->RunLabel(szMapadd, "ZombieSurvival");
		}
		else
		{
			pMapadd->RunLabel(szMapadd, "ZombieSurvival");
		}
	}
	else if (g_pGameRules->GetGamemode() == FIREFIGHT_PRIMARY_FIREFIGHTRUMBLE)
	{
		CMapAdd* pMapadd = GetMapAddEntity();
		char szMapadd[128];
		Q_snprintf(szMapadd, sizeof(szMapadd), "mapadd/%s.txt", STRING(gpGlobals->mapname));
		if (!pMapadd)
		{
			pMapadd = CreateMapAddEntity();
			pMapadd->RunLabel(szMapadd, "FirefightRumble");
		}
		else
		{
			pMapadd->RunLabel(szMapadd, "FirefightRumble");
		}
	}
}
