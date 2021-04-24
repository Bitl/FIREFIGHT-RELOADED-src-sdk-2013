//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: An entity that creates NPCs in the game.
//
//=============================================================================//

#include "cbase.h"
#include "datacache/imdlcache.h"
#include "entityapi.h"
#include "entityoutput.h"
#include "ai_basenpc.h"
#include "monstermaker_firefight.h"
#include "TemplateEntities.h"
#include "ndebugoverlay.h"
#include "mapentities.h"
#include "IEffects.h"
#include "props.h"
#include "npc_metropolice.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sk_initialspawnertime("sk_initialspawnertime", "5", FCVAR_CHEAT);
ConVar sk_spawnrareenemies("sk_spawnrareenemies", "1", FCVAR_ARCHIVE);
ConVar sk_spawnerhidefromplayer("sk_spawnerhidefromplayer", "1", FCVAR_ARCHIVE);
//ConVar sk_spawnerminclientstospawn("sk_spawnerminclientstospawn", "2", FCVAR_NOTIFY);

const char *g_CombineSoldierWeapons[] =
{
	"weapon_smg1",
	"weapon_ar2",
	"weapon_shotgun"
};

const char *g_MetropoliceWeapons[] =
{
	"weapon_smg1",
	"weapon_pistol",
	"weapon_stunstick"
};

const char *g_EliteMetropoliceWeapons[] =
{
	"weapon_smg1",
	"weapon_ar2",
	"weapon_pistol"
};

const char *g_charNPCSCommon[] =
{
	"npc_metropolice",
	"npc_combine_s",
	"npc_combine_e",
	"npc_combine_p",
	"npc_combine_shot",
	"npc_antlion"
	"npc_zombie",
	"npc_zombie_torso",
	"npc_fastzombie",
	"npc_fastzombie_torso",
	"npc_agrunt",
	"npc_houndeye",
	"npc_bullsquid",
	"npc_elitepolice",
	"npc_acontroller"
};

const char *g_charNPCSRare[] =
{
	"npc_combine_ace",
	"npc_hunter",
	"npc_antlionworker",
	"npc_antlionguard",
	"npc_antlionguardian",
	"npc_poisonzombie",
	"npc_zombine",
	"npc_headcrab_fast",
	"npc_headcrab_poison",
	"npc_vortigaunt",
	"npc_assassin",
	"npc_cremator",
	"npc_headcrab",
	"npc_combineguard",
	"npc_ministrider",
	"npc_rollermine",
	"npc_cscanner",
	//temp until we actually set up the wave system.
	"npc_player"
};

const char* g_charNPCSCombineFirefightCommon[] =
{
	"npc_metropolice",
	"npc_combine_s",
	"npc_combine_e",
	"npc_combine_p",
	"npc_combine_shot",
	"npc_elitepolice"
};

const char* g_charNPCSCombineFirefightRare[] =
{
	"npc_combine_ace",
	"npc_hunter",
	"npc_assassin",
	"npc_cremator",
	"npc_combineguard",
	"npc_ministrider",
	"npc_rollermine",
	"npc_cscanner",
	//temp until we actually set up the wave system.
	"npc_player"
};

//why.
const char* g_charNPCSAntlionAssaultCommon[] =
{
	"npc_antlion"
};

const char* g_charNPCSAntlionAssaultRare[] =
{
	"npc_antlionworker",
	"npc_antlionguard",
	"npc_antlionguardian",
	//temp until we actually set up the wave system.
	"npc_player"
};

const char* g_charNPCSZombieSurvivalCommon[] =
{
	"npc_zombie",
	"npc_zombie_torso",
	"npc_fastzombie",
	"npc_fastzombie_torso"
};

const char* g_charNPCSZombieSurvivalRare[] =
{
	"npc_poisonzombie",
	"npc_zombine",
	//temp until we actually set up the wave system.
	"npc_player"
};

const char* g_charNPCSXenInvasionCommon[] =
{
	"npc_houndeye",
	"npc_bullsquid",
	"npc_agrunt",
	"npc_acontroller"
};

const char* g_charNPCSXenInvasionRare[] =
{
	"npc_headcrab_fast",
	"npc_headcrab_poison",
	"npc_vortigaunt",
	"npc_headcrab",
	//temp until we actually set up the wave system.
	"npc_player"
};

static void DispatchActivate( CBaseEntity *pEntity )
{
	bool bAsyncAnims = mdlcache->SetAsyncLoad( MDLCACHE_ANIMBLOCK, false );
	pEntity->Activate();
	mdlcache->SetAsyncLoad( MDLCACHE_ANIMBLOCK, bAsyncAnims );
}

LINK_ENTITY_TO_CLASS(npc_maker_firefight, CNPCMakerFirefight);

//-------------------------------------
BEGIN_DATADESC(CNPCMakerFirefight)
	
	DEFINE_KEYFIELD( m_nMaxLiveChildren,		FIELD_INTEGER,	"MaxLiveChildren" ),
	DEFINE_KEYFIELD( m_flSpawnFrequency,		FIELD_FLOAT,	"SpawnFrequency" ),
	DEFINE_KEYFIELD( m_bDisabled,			FIELD_BOOLEAN,	"StartDisabled" ),
	DEFINE_KEYFIELD( m_iszNPCClassname, FIELD_STRING, "NPCType" ),
	DEFINE_KEYFIELD( m_ChildTargetName, FIELD_STRING, "NPCTargetname" ),
	DEFINE_KEYFIELD( m_SquadName, FIELD_STRING, "NPCSquadName" ),
	DEFINE_KEYFIELD( m_spawnEquipment, FIELD_STRING, "additionalequipment" ),
	DEFINE_KEYFIELD( m_strHintGroup, FIELD_STRING, "NPCHintGroup" ),
	DEFINE_KEYFIELD( m_RelationshipString, FIELD_STRING, "Relationship" ),
	DEFINE_KEYFIELD( m_nMaxLiveRareNPCs, FIELD_INTEGER, "MaxLiveRareNPCs" ),
	DEFINE_KEYFIELD( m_nRareNPCRarity, FIELD_INTEGER, "RareNPCRarity" ),

	DEFINE_FIELD(	m_nLiveChildren,		FIELD_INTEGER ),
	DEFINE_FIELD(	m_nLiveRareNPCs,		FIELD_INTEGER ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID,	"Spawn",	InputSpawnNPC ),
	DEFINE_INPUTFUNC( FIELD_VOID,	"SpawnRare", InputSpawnRareNPC ),
	DEFINE_INPUTFUNC( FIELD_VOID,	"Enable",	InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID,	"Disable",	InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID,	"Toggle",	InputToggle ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetMaxLiveChildren", InputSetMaxLiveChildren ),
	DEFINE_INPUTFUNC( FIELD_FLOAT,	 "SetSpawnFrequency", InputSetSpawnFrequency ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetRareNPCRarity", InputSetRareNPCRarity ),

	// Outputs
	DEFINE_OUTPUT( m_OnAllLiveChildrenDead,	"OnAllLiveChildrenDead" ),
	DEFINE_OUTPUT( m_OnSpawnNPC,		"OnSpawnNPC" ),
	DEFINE_OUTPUT( m_OnSpawnRareNPC,		"OnSpawnRareNPC" ),

	// Function Pointers
	DEFINE_THINKFUNC( MakerThink ),

	DEFINE_FIELD( m_hIgnoreEntity, FIELD_EHANDLE ),
	DEFINE_KEYFIELD( m_iszIngoreEnt, FIELD_STRING, "IgnoreEntity" ), 
END_DATADESC()

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CNPCMakerFirefight::CNPCMakerFirefight(void)
{
	m_spawnEquipment = NULL_STRING;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::Spawn(void)
{

	SetSolid( SOLID_NONE );
	m_nLiveChildren		= 0;
	m_nLiveRareNPCs		= 0;
	Precache();

	m_spawnflags |= SF_NPCMAKER_FADE;

	//Start on?
	if ( m_bDisabled == false )
	{
		SetThink(&CNPCMakerFirefight::MakerThink);
		SetNextThink(gpGlobals->curtime + sk_initialspawnertime.GetFloat());
	}
	else
	{
		//wait to be activated.
		SetThink(&CNPCMakerFirefight::SUB_DoNothing);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Precache the target NPC
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::Precache(void)
{
	BaseClass::Precache();

	//precache both common and rare npc lists instead of a fucking HUGE list.
	int nNPCsPrecacheCommon = ARRAYSIZE(g_charNPCSCommon);
	for (int i = 0; i < nNPCsPrecacheCommon; ++i)
	{
		UTIL_PrecacheOther(g_charNPCSCommon[i]);
		DevMsg("Precaching common NPC %s.\n", g_charNPCSCommon[i]);
	}

	int nNPCsPrecacheRare = ARRAYSIZE(g_charNPCSRare);
	for (int i = 0; i < nNPCsPrecacheRare; ++i)
	{
		UTIL_PrecacheOther(g_charNPCSRare[i]);
		DevMsg("Precaching rare NPC %s.\n", g_charNPCSRare[i]);
	}

	//precache weapon lists too, just in case.
	int nCombineSoldierWeapons = ARRAYSIZE(g_CombineSoldierWeapons);
	for (int i = 0; i < nCombineSoldierWeapons; ++i)
	{
		UTIL_PrecacheOther(g_CombineSoldierWeapons[i]);
		DevMsg("Precaching Combine Soldier weapon %s.\n", g_CombineSoldierWeapons[i]);
	}

	int nMetropoliceWeapons = ARRAYSIZE(g_MetropoliceWeapons);
	for (int i = 0; i < nMetropoliceWeapons; ++i)
	{
		UTIL_PrecacheOther(g_MetropoliceWeapons[i]);
		DevMsg("Precaching Civil Protection weapon %s.\n", g_MetropoliceWeapons[i]);
	}

	int nEliteMetropoliceWeapons = ARRAYSIZE(g_EliteMetropoliceWeapons);
	for (int i = 0; i < nEliteMetropoliceWeapons; ++i)
	{
		UTIL_PrecacheOther(g_EliteMetropoliceWeapons[i]);
		DevMsg("Precaching Elite Civil Protection weapon %s.\n", g_EliteMetropoliceWeapons[i]);
	}

	/*
	const char *pszNPCName = STRING(m_iszNPCClassname);
	if (!pszNPCName || !pszNPCName[0])
	{
		Warning("npc_maker_firefight %s has no specified NPC-to-spawn classname.\n", STRING(GetEntityName()));
	}
	else
	{
		UTIL_PrecacheOther(pszNPCName);
	}
	*/
}

//-----------------------------------------------------------------------------
// Purpose: Creates a new NPC every so often.
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::MakerThink(void)
{
	if (g_fr_spawneroldfunctionality.GetBool())
	{
		if (g_pGameRules->GetGamemode() == FIREFIGHT_PRIMARY_ANTLIONASSAULT)
		{
			SetNextThink(gpGlobals->curtime + (m_flSpawnFrequency + 15));
		}
		else
		{
			SetNextThink(gpGlobals->curtime + m_flSpawnFrequency);
		}
	}
	else
	{
		SetNextThink(gpGlobals->curtime + m_flSpawnFrequency);
	}

	if (sk_spawnrareenemies.GetBool())
	{
		int rarenpcrandom = random->RandomInt(0, m_nRareNPCRarity);

		if (HasSpawnFlags(SF_NPCMAKER_DOUBLETROUBLE))
		{
			if (rarenpcrandom == 0 && CanMakeRareNPC())
			{
				MakeNPC(true);
			}
			else
			{
				MakeNPC();
				SetNextThink(gpGlobals->curtime + 0.5);
				MakeNPC();
			}
		}
		else
		{
			if (rarenpcrandom == 0 && CanMakeRareNPC())
			{
				MakeNPC(true);
			}
			else
			{
				MakeNPC();
			}
		}
	}
	else
	{
		if (HasSpawnFlags(SF_NPCMAKER_DOUBLETROUBLE))
		{
			MakeNPC();
			SetNextThink(gpGlobals->curtime + 0.5);
			MakeNPC();
		}
		else
		{
			MakeNPC();
		}
	}
}

//-----------------------------------------------------------------------------
// A not-very-robust check to see if a human hull could fit at this location.
// used to validate spawn destinations.
//-----------------------------------------------------------------------------
bool CNPCMakerFirefight::HumanHullFits(const Vector &vecLocation)
{
	trace_t tr;
	UTIL_TraceHull( vecLocation,
					vecLocation + Vector( 0, 0, 1 ),
					NAI_Hull::Mins(HULL_HUMAN),
					NAI_Hull::Maxs(HULL_HUMAN),
					MASK_NPCSOLID,
					m_hIgnoreEntity,
					COLLISION_GROUP_NONE,
					&tr );

	if( tr.fraction == 1.0 )
		return true;

	return false;
}

#define ENTITY_INTOLERANCE	100

//-----------------------------------------------------------------------------
// Purpose: Returns whether or not it is OK to make an NPC at this instant.
//-----------------------------------------------------------------------------
bool CNPCMakerFirefight::CanMakeNPC(bool bIgnoreSolidEntities)
{
	if ( gEntList.NumberOfEntities() >= (gpGlobals->maxEntities - ENTITY_INTOLERANCE) )
		return false;

	if (g_iNPCLimit >= g_fr_npclimit.GetInt() && g_fr_npclimit.GetInt() != 0)
		return false;

	/*
	int iMinPlayersToSpawn = 0;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer	*pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer != NULL)
		{
			iMinPlayersToSpawn++;
		}
	}

	if (iMinPlayersToSpawn < sk_spawnerminclientstospawn.GetInt() && g_pGameRules->IsMultiplayer())
		return false;*/

	if ( m_nMaxLiveChildren > 0 && m_nLiveChildren >= m_nMaxLiveChildren )
	{// not allowed to make a new one yet. Too many live ones out right now.
		return false;
	}

	if ( m_iszIngoreEnt != NULL_STRING )
	{
		m_hIgnoreEntity = gEntList.FindEntityByName( NULL, m_iszIngoreEnt );
	}

	Vector mins = GetAbsOrigin() - Vector( 34, 34, 0 );
	Vector maxs = GetAbsOrigin() + Vector( 34, 34, 0 );
	maxs.z = GetAbsOrigin().z;
	
	// If we care about not hitting solid entities, look for 'em
	if ( !bIgnoreSolidEntities )
	{
		CBaseEntity *pList[128];

		int count = UTIL_EntitiesInBox( pList, 128, mins, maxs, FL_CLIENT|FL_NPC );
		if ( count )
		{
			//Iterate through the list and check the results
			for ( int i = 0; i < count; i++ )
			{
				//Don't build on top of another entity
				if ( pList[i] == NULL )
					continue;

				//If one of the entities is solid, then we may not be able to spawn now
				if ( ( pList[i]->GetSolidFlags() & FSOLID_NOT_SOLID ) == false )
				{
					// Since the outer method doesn't work well around striders on account of their huge bounding box.
					// Find the ground under me and see if a human hull would fit there.
					trace_t tr;
					UTIL_TraceHull( GetAbsOrigin() + Vector( 0, 0, 2 ),
									GetAbsOrigin() - Vector( 0, 0, 8192 ),
									NAI_Hull::Mins(HULL_HUMAN),
									NAI_Hull::Maxs(HULL_HUMAN),
									MASK_NPCSOLID,
									m_hIgnoreEntity,
									COLLISION_GROUP_NONE,
									&tr );

					if( !HumanHullFits( tr.endpos + Vector( 0, 0, 1 ) ) )
					{
						return false;
					}
				}
			}
		}
	}

	// Do we need to check to see if the player's looking?
	if (sk_spawnerhidefromplayer.GetBool())
	{
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);
			if ( pPlayer )
			{
				// Only spawn if the player's looking away from me
				if( pPlayer->FInViewCone( GetAbsOrigin() ) && pPlayer->FVisible( GetAbsOrigin() ) )
				{
					if ( !(pPlayer->GetFlags() & FL_NOTARGET) )
						return false;
					DevMsg( 2, "Spawner %s spawning even though seen due to notarget\n", STRING( GetEntityName() ) );
				}
			}
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Returns whether or not it is OK to make an NPC at this instant.
//-----------------------------------------------------------------------------
bool CNPCMakerFirefight::CanMakeRareNPC()
{
	if (gEntList.NumberOfEntities() >= (gpGlobals->maxEntities - ENTITY_INTOLERANCE))
		return false;

	if (g_iNPCLimit >= g_fr_npclimit.GetInt() && g_fr_npclimit.GetInt() != 0)
		return false;

	if (m_nMaxLiveRareNPCs > 0 && m_nLiveRareNPCs >= m_nMaxLiveRareNPCs)
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pVictim - 
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::DeathNotice(CBaseEntity *pVictim)
{
	// ok, we've gotten the deathnotice from our child, now clear out its owner if we don't want it to fade.
	m_nLiveChildren--;
	g_iNPCLimit--;

	// If we're here, we're getting erroneous death messages from children we haven't created
	AssertMsg(m_nLiveChildren >= 0, "npc_maker_firefight receiving child death notice but thinks has no children\n");

	if (pVictim->m_isRareEntity)
	{
		// ok, we've gotten the deathnotice from our child, now clear out its owner if we don't want it to fade.
		m_nLiveRareNPCs--;

		// If we're here, we're getting erroneous death messages from children we haven't created
		AssertMsg(m_nLiveRareNPCs >= 0, "npc_maker_firefight receiving child death notice but thinks has no children (RARE)\n");
	}
}

const char** CNPCMakerFirefight::GetListForRareNPCs(void)
{
	switch (g_pGameRules->GetGamemode())
	{
		case FIREFIGHT_PRIMARY_COMBINEFIREFIGHT:
			return g_charNPCSCombineFirefightRare;
		case FIREFIGHT_PRIMARY_XENINVASION:
			return g_charNPCSXenInvasionRare;
		case FIREFIGHT_PRIMARY_ANTLIONASSAULT:
			return g_charNPCSAntlionAssaultRare;
		case FIREFIGHT_PRIMARY_ZOMBIESURVIVAL:
			return g_charNPCSZombieSurvivalRare;
		case FIREFIGHT_PRIMARY_FIREFIGHTRUMBLE:
		case FIREFIGHT_PRIMARY_DEFAULT:
		default:
			return g_charNPCSRare;
	}
}

const char** CNPCMakerFirefight::GetListForCommonNPCs(void)
{
	switch (g_pGameRules->GetGamemode())
	{
		case FIREFIGHT_PRIMARY_COMBINEFIREFIGHT:
			return g_charNPCSCombineFirefightCommon;
		case FIREFIGHT_PRIMARY_XENINVASION:
			return g_charNPCSXenInvasionCommon;
		case FIREFIGHT_PRIMARY_ANTLIONASSAULT:
			return g_charNPCSAntlionAssaultCommon;
		case FIREFIGHT_PRIMARY_ZOMBIESURVIVAL:
			return g_charNPCSZombieSurvivalCommon;
		case FIREFIGHT_PRIMARY_FIREFIGHTRUMBLE:
		case FIREFIGHT_PRIMARY_DEFAULT:
		default:
			return g_charNPCSCommon;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Creates the NPC.
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::MakeNPC(bool rareNPC)
{
	if (!CanMakeNPC())
		return;

	const char* pRandomName = "npc_player";
	
	if (rareNPC)
	{
		const char** entList = GetListForRareNPCs();
		int randomChoice = rand() % sizeof(entList);
		pRandomName = entList[randomChoice];
	}
	else
	{
		const char** entList = GetListForCommonNPCs();
		int randomChoice = rand() % sizeof(entList);
		pRandomName = entList[randomChoice];
	}

	CAI_BaseNPC* pent = (CAI_BaseNPC*)CreateEntityByName(pRandomName);

	if (!pent)
	{
		Warning("npc_maker_firefight: Entity classname does not exist in database.\n");
		return;
	}

	// ------------------------------------------------
	//  Intialize spawned NPC's relationships
	// ------------------------------------------------
	pent->SetRelationshipString(m_RelationshipString);

	m_OnSpawnNPC.Set(pent, pent, this);

	pent->SetAbsOrigin(GetAbsOrigin());

	// Strip pitch and roll from the spawner's angles. Pass only yaw to the spawned NPC.
	QAngle angles = GetAbsAngles();
	angles.x = 0.0;
	angles.z = 0.0;
	pent->SetAbsAngles(angles);

	pent->AddSpawnFlags(SF_NPC_FALL_TO_GROUND);

	if (m_spawnflags & SF_NPCMAKER_FADE)
	{
		pent->AddSpawnFlags(SF_NPC_FADE_CORPSE);
	}

	if (Q_stristr(pRandomName, "npc_metropolice"))
	{
		int nWeaponsPolice = ARRAYSIZE(g_MetropoliceWeapons);
		int randomChoicePolice = rand() % nWeaponsPolice;
		const char* pRandomNamePolice = g_MetropoliceWeapons[randomChoicePolice];
		pent->m_spawnEquipment = MAKE_STRING(pRandomNamePolice);
		pent->AddSpawnFlags(SF_METROPOLICE_ALLOWED_TO_RESPOND);
		pent->AddSpawnFlags(SF_METROPOLICE_MID_RANGE_ATTACK);
	}
	else if (Q_stristr(pRandomName, "npc_elitepolice"))
	{
		int nWeaponsElitePolice = ARRAYSIZE(g_EliteMetropoliceWeapons);
		int randomChoiceElitePolice = rand() % nWeaponsElitePolice;
		const char* pRandomNameElitePolice = g_EliteMetropoliceWeapons[randomChoiceElitePolice];
		pent->m_spawnEquipment = MAKE_STRING(pRandomNameElitePolice);
	}
	else if (Q_stristr(pRandomName, "npc_combine_shot"))
	{
		pent->m_spawnEquipment = MAKE_STRING("weapon_shotgun");
	}
	else if (Q_stristr(pRandomName, "npc_combine_s"))
	{
		int nWeaponsSoldier = ARRAYSIZE(g_CombineSoldierWeapons);
		int randomChoiceSoldier = rand() % nWeaponsSoldier;
		const char* pRandomNameSoldier = g_CombineSoldierWeapons[randomChoiceSoldier];
		pent->m_spawnEquipment = MAKE_STRING(pRandomNameSoldier);
	}
	else if (Q_stristr(pRandomName, "npc_combine_e"))
	{
		int nWeaponsSoldier = ARRAYSIZE(g_CombineSoldierWeapons);
		int randomChoiceSoldier = rand() % nWeaponsSoldier;
		const char* pRandomNameSoldier = g_CombineSoldierWeapons[randomChoiceSoldier];
		pent->m_spawnEquipment = MAKE_STRING(pRandomNameSoldier);
	}
	else if (Q_stristr(pRandomName, "npc_combine_p"))
	{
		int nWeaponsSoldier = ARRAYSIZE(g_CombineSoldierWeapons);
		int randomChoiceSoldier = rand() % nWeaponsSoldier;
		const char* pRandomNameSoldier = g_CombineSoldierWeapons[randomChoiceSoldier];
		pent->m_spawnEquipment = MAKE_STRING(pRandomNameSoldier);
	}
	else if (Q_stristr(pRandomName, "npc_combine_ace"))
	{
		pent->m_spawnEquipment = MAKE_STRING("weapon_smg1");
	}
	else if (Q_stristr(pRandomName, "npc_player"))
	{
		//alert all players.
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer* pPlayer = UTIL_PlayerByIndex(i);
			if (pPlayer)
			{
				CFmtStr hint;
				hint.sprintf("#Valve_Hud_AllySpawned");
				pPlayer->ShowLevelMessage(hint.Access());
			}
		}
	}

	pent->m_isRareEntity = rareNPC;
	pent->SetSquadName(m_SquadName);
	pent->SetHintGroup(m_strHintGroup);

	ChildPreSpawn(pent);

	DispatchSpawn(pent);
	pent->SetOwnerEntity(this);
	DispatchActivate(pent);

	if (m_ChildTargetName != NULL_STRING)
	{
		// if I have a netname (overloaded), give the child NPC that name as a targetname
		pent->SetName(m_ChildTargetName);
	}

	ChildPostSpawn(pent);

	if (rareNPC)
	{
		//rare entities have their own value we must consider.
		m_nLiveRareNPCs++;
	}
	m_nLiveChildren++;// count this NPC
	g_iNPCLimit++;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pChild - 
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::ChildPostSpawn(CAI_BaseNPC *pChild)
{
	// If I'm stuck inside any props, remove them
	bool bFound = true;
	while (bFound)
	{
		trace_t tr;
		UTIL_TraceHull(pChild->GetAbsOrigin(), pChild->GetAbsOrigin(), pChild->WorldAlignMins(), pChild->WorldAlignMaxs(), MASK_NPCSOLID, pChild, COLLISION_GROUP_NONE, &tr);
		//NDebugOverlay::Box( pChild->GetAbsOrigin(), pChild->WorldAlignMins(), pChild->WorldAlignMaxs(), 0, 255, 0, 32, 5.0 );
		if (tr.fraction != 1.0 && tr.m_pEnt)
		{
			if (FClassnameIs(tr.m_pEnt, "prop_physics"))
			{
				// Set to non-solid so this loop doesn't keep finding it
				tr.m_pEnt->AddSolidFlags(FSOLID_NOT_SOLID);
				UTIL_RemoveImmediate(tr.m_pEnt);
				continue;
			}
		}

		bFound = false;
	}
	if (m_hIgnoreEntity != NULL)
	{
		pChild->SetOwnerEntity(m_hIgnoreEntity);
	}
}


//-----------------------------------------------------------------------------
// Purpose: If this had a finite number of children, return true if they've all
//			been created.
//-----------------------------------------------------------------------------
bool CNPCMakerFirefight::IsDepleted()
{
	return false;
}


//-----------------------------------------------------------------------------
// Purpose: Toggle the spawner's state
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::Toggle(void)
{
	if ( m_bDisabled )
	{
		Enable();
	}
	else
	{
		Disable();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Start the spawner
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::Enable(void)
{
	m_bDisabled = false;
	SetThink(&CNPCMakerFirefight::MakerThink);
	SetNextThink( gpGlobals->curtime );
}


//-----------------------------------------------------------------------------
// Purpose: Stop the spawner
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::Disable(void)
{
	m_bDisabled = true;
	SetThink ( NULL );
}


//-----------------------------------------------------------------------------
// Purpose: Input handler that spawns an NPC.
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::InputSpawnNPC(inputdata_t &inputdata)
{
	MakeNPC();
}

//-----------------------------------------------------------------------------
// Purpose: Input handler that spawns an NPC.
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::InputSpawnRareNPC(inputdata_t &inputdata)
{
	MakeNPC(true);
}

//-----------------------------------------------------------------------------
// Purpose: Input hander that starts the spawner
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::InputEnable(inputdata_t &inputdata)
{
	Enable();
}


//-----------------------------------------------------------------------------
// Purpose: Input hander that stops the spawner
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::InputDisable(inputdata_t &inputdata)
{
	Disable();
}


//-----------------------------------------------------------------------------
// Purpose: Input hander that toggles the spawner
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::InputToggle(inputdata_t &inputdata)
{
	Toggle();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::InputSetMaxLiveChildren(inputdata_t &inputdata)
{
	m_nMaxLiveChildren = inputdata.value.Int();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPCMakerFirefight::InputSetMaxLiveRareNPCS(inputdata_t &inputdata)
{
	m_nMaxLiveRareNPCs = inputdata.value.Int();
}

void CNPCMakerFirefight::InputSetSpawnFrequency(inputdata_t &inputdata)
{
	m_flSpawnFrequency = inputdata.value.Float();
}

void CNPCMakerFirefight::InputSetRareNPCRarity(inputdata_t &inputdata)
{
	m_nRareNPCRarity = inputdata.value.Int();
}
