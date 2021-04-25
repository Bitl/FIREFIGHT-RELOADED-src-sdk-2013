//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "singleplay_gamerules.h"
#include "fmtstr.h"

#ifdef CLIENT_DLL

#else

	#include "player.h"
	#include "basecombatweapon.h"
	#include "gamerules.h"
	#include "game.h"
	#include "items.h"
	#include "SMMOD/mapadd.h"
	#include "iscorer.h"

#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sk_money_multiplier1("sk_money_multiplier1", "4");
ConVar sk_money_multiplier2("sk_money_multiplier2", "4");
ConVar sk_money_multiplier3("sk_money_multiplier3", "3");
ConVar sk_money_multiplier4("sk_money_multiplier4", "2");
ConVar sk_money_multiplier5("sk_money_multiplier5", "1");

ConVar sk_exp_multiplier1("sk_exp_multiplier1", "4");
ConVar sk_exp_multiplier2("sk_exp_multiplier2", "4");
ConVar sk_exp_multiplier3("sk_exp_multiplier3", "3");
ConVar sk_exp_multiplier4("sk_exp_multiplier4", "2");
ConVar sk_exp_multiplier5("sk_exp_multiplier5", "1");

ConVar sv_killingspree("sv_killingspree", "1", FCVAR_ARCHIVE);

ConVar sv_healthcharger_recharge("sv_healthcharger_recharge", "1", FCVAR_ARCHIVE);
ConVar sv_healthcharger_recharge_time("sv_healthcharger_recharge_time", "180", FCVAR_CHEAT);
ConVar sv_suitcharger_recharge("sv_suitcharger_recharge", "1", FCVAR_ARCHIVE);
ConVar sv_suitcharger_recharge_time("sv_suitcharger_recharge_time", "180", FCVAR_CHEAT);
ConVar sv_item_respawn("sv_item_respawn", "1", FCVAR_ARCHIVE);
ConVar sv_item_respawn_time("sv_item_respawn_time", "180", FCVAR_CHEAT);
ConVar sv_weapon_respawn("sv_weapon_respawn", "1", FCVAR_ARCHIVE);
ConVar sv_weapon_respawn_time("sv_weapon_respawn_time", "180", FCVAR_CHEAT);
ConVar sv_player_dropweaponsondeath("sv_player_dropweaponsondeath", "1", FCVAR_ARCHIVE);

extern ConVar sv_player_voice;
extern ConVar sv_player_voice_kill_freq;
extern ConVar sv_player_voice_kill;

//=========================================================
//=========================================================
bool CSingleplayRules::IsMultiplayer( void )
{
	return gpGlobals->maxClients > 1;
}

// Needed during the conversion, but once DMG_* types have been fixed, this isn't used anymore.
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CSingleplayRules::Damage_GetTimeBased( void )
{
	int iDamage = ( DMG_PARALYZE | DMG_NERVEGAS | DMG_POISON | DMG_RADIATION | DMG_DROWNRECOVER | DMG_ACID | DMG_SLOWBURN );
	return iDamage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CSingleplayRules::Damage_GetShouldGibCorpse( void )
{
	int iDamage = ( DMG_CRUSH | DMG_FALL | DMG_BLAST | DMG_SONIC | DMG_CLUB );
	return iDamage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CSingleplayRules::Damage_GetShowOnHud( void )
{
	int iDamage = ( DMG_POISON | DMG_ACID | DMG_DROWN | DMG_BURN | DMG_SLOWBURN | DMG_NERVEGAS | DMG_RADIATION | DMG_SHOCK );
	return iDamage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CSingleplayRules::Damage_GetNoPhysicsForce( void )
{
	int iTimeBasedDamage = Damage_GetTimeBased();
	int iDamage = ( DMG_FALL | DMG_BURN | DMG_PLASMA | DMG_DROWN | iTimeBasedDamage | DMG_CRUSH | DMG_PHYSGUN | DMG_PREVENT_PHYSICS_FORCE );
	return iDamage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CSingleplayRules::Damage_GetShouldNotBleed( void )
{
	int iDamage = ( DMG_POISON | DMG_ACID );
	return iDamage;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : iDmgType - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CSingleplayRules::Damage_IsTimeBased( int iDmgType )
{
	// Damage types that are time-based.
	return ( ( iDmgType & ( DMG_PARALYZE | DMG_NERVEGAS | DMG_POISON | DMG_RADIATION | DMG_DROWNRECOVER | DMG_ACID | DMG_SLOWBURN ) ) != 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : iDmgType - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CSingleplayRules::Damage_ShouldGibCorpse( int iDmgType )
{
	// Damage types that gib the corpse.
	return ( ( iDmgType & ( DMG_CRUSH | DMG_FALL | DMG_BLAST | DMG_SONIC | DMG_CLUB ) ) != 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : iDmgType - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CSingleplayRules::Damage_ShowOnHUD( int iDmgType )
{
	// Damage types that have client HUD art.
	return ( ( iDmgType & ( DMG_POISON | DMG_ACID | DMG_DROWN | DMG_BURN | DMG_SLOWBURN | DMG_NERVEGAS | DMG_RADIATION | DMG_SHOCK ) ) != 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : iDmgType - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CSingleplayRules::Damage_NoPhysicsForce( int iDmgType )
{
	// Damage types that don't have to supply a physics force & position.
	int iTimeBasedDamage = Damage_GetTimeBased();
	return ( ( iDmgType & ( DMG_FALL | DMG_BURN | DMG_PLASMA | DMG_DROWN | iTimeBasedDamage | DMG_CRUSH | DMG_PHYSGUN | DMG_PREVENT_PHYSICS_FORCE ) ) != 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : iDmgType - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CSingleplayRules::Damage_ShouldNotBleed( int iDmgType )
{
	// Damage types that don't make the player bleed.
	return ( ( iDmgType & ( DMG_POISON | DMG_ACID ) ) != 0 );
}

#ifdef CLIENT_DLL

#else

	extern CGameRules	*g_pGameRules;
	extern bool		g_fGameOver;

	//=========================================================
	//=========================================================
	CSingleplayRules::CSingleplayRules( void )
	{
		RefreshSkillData( true );

		char mapcfg[256];
		Q_snprintf(mapcfg, sizeof(mapcfg), "cfg/%s.cfg", STRING(gpGlobals->mapname));

		Q_FixSlashes(mapcfg);
		Q_strlower(mapcfg);

		if (mapcfg)
		{
			char szCommandMap[256];

			Log("Executing map config file %s\n", mapcfg);
			Q_snprintf(szCommandMap, sizeof(szCommandMap), "exec %s\n", mapcfg);
			engine->ServerCommand(szCommandMap);
		}

		char mapname[256];
#if !defined( CLIENT_DLL )
		Q_snprintf(mapname, sizeof(mapname), "maps/%s", STRING(gpGlobals->mapname));
#else
		Q_strncpy(mapname, engine->GetLevelName(), sizeof(mapname));
#endif

		Q_FixSlashes(mapname);
		Q_strlower(mapname);

		if (g_fr_spawneroldfunctionality.GetBool())
		{
			if (V_stristr(mapname, "cf"))
			{
				SetGamemode(FIREFIGHT_PRIMARY_COMBINEFIREFIGHT);
				Log("Automatically setting the gamemode to COMBINE FIREFIGHT due to mapname.\n");
			}
			else if (V_stristr(mapname, "xi"))
			{
				SetGamemode(FIREFIGHT_PRIMARY_XENINVASION);
				Log("Automatically setting the gamemode to XEN INVASION due to mapname.\n");
			}
			else if (V_stristr(mapname, "aa"))
			{
				SetGamemode(FIREFIGHT_PRIMARY_ANTLIONASSAULT);
				Log("Automatically setting the gamemode to ANTLION ASSAULT due to mapname.\n");
			}
			else if (V_stristr(mapname, "zs"))
			{
				SetGamemode(FIREFIGHT_PRIMARY_ZOMBIESURVIVAL);
				Log("Automatically setting the gamemode to ZOMBIE SURVIVAL due to mapname.\n");
			}
			else if (V_stristr(mapname, "fr"))
			{
				SetGamemode(FIREFIGHT_PRIMARY_FIREFIGHTRUMBLE);
				Log("Automatically setting the gamemode to FIREFIGHT RUMBLE due to mapname.\n");
			}

			if (GetGamemode() == FIREFIGHT_PRIMARY_DEFAULT)
			{
				if (bHasRandomized)
				{
					bHasRandomized = false;
					iRandomGamemode = 0;
				}
				Log("No gamemode defined! Randomizing gamemodes.\n");
				SetGamemodeRandom(FIREFIGHT_PRIMARY_COMBINEFIREFIGHT, FIREFIGHT_PRIMARY_FIREFIGHTRUMBLE, true);
				bHasRandomized = true;
			}

			const char* cfgfilecf = combinefirefightcfgfile.GetString();
			const char* cfgfilexi = xeninvasioncfgfile.GetString();
			const char* cfgfileaa = antlionassaultcfgfile.GetString();
			const char* cfgfilezs = zombiesurvivalcfgfile.GetString();
			const char* cfgfilefr = firefightrumblecfgfile.GetString();

			switch (g_pGameRules->GetGamemode())
			{
				case FIREFIGHT_PRIMARY_COMBINEFIREFIGHT:
					if (cfgfilecf && cfgfilecf[0])
					{
						char szCommand[256];

						Log("Executing COMBINE FIREFIGHT gamemode config file %s\n", cfgfilecf);
						Q_snprintf(szCommand, sizeof(szCommand), "exec %s\n", cfgfilecf);
						engine->ServerCommand(szCommand);
					}
					break;
				case FIREFIGHT_PRIMARY_XENINVASION:
					if (cfgfilexi && cfgfilexi[0])
					{
						char szCommand[256];

						Log("Executing XEN INVASION gamemode config file %s\n", cfgfilexi);
						Q_snprintf(szCommand, sizeof(szCommand), "exec %s\n", cfgfilexi);
						engine->ServerCommand(szCommand);
					}
					break;
				case FIREFIGHT_PRIMARY_ANTLIONASSAULT:
					if (cfgfileaa && cfgfileaa[0])
					{
						char szCommand[256];

						Log("Executing ANTLION ASSAULT gamemode config file %s\n", cfgfileaa);
						Q_snprintf(szCommand, sizeof(szCommand), "exec %s\n", cfgfileaa);
						engine->ServerCommand(szCommand);
					}
					break;
				case FIREFIGHT_PRIMARY_ZOMBIESURVIVAL:
					if (cfgfilezs && cfgfilezs[0])
					{
						char szCommand[256];

						Log("Executing ZOMBIE SURVIVAL gamemode config file %s\n", cfgfilezs);
						Q_snprintf(szCommand, sizeof(szCommand), "exec %s\n", cfgfilezs);
						engine->ServerCommand(szCommand);
					}
					break;
				case FIREFIGHT_PRIMARY_FIREFIGHTRUMBLE:
				default:
					if (cfgfilefr && cfgfilefr[0])
					{
						char szCommand[256];

						Log("Executing FIREFIGHT RUMBLE gamemode config file %s\n", cfgfilefr);
						Q_snprintf(szCommand, sizeof(szCommand), "exec %s\n", cfgfilefr);
						engine->ServerCommand(szCommand);
					}
					break;
			}
		}
	}

	//=========================================================
	//=========================================================
	void CSingleplayRules::Think ( void )
	{
		BaseClass::Think();
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::IsDeathmatch ( void )
	{
		return false;
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::IsCoOp( void )
	{
		return gpGlobals->maxClients > 1;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Determine whether the player should switch to the weapon passed in
	// Output : Returns true on success, false on failure.
	//-----------------------------------------------------------------------------
	bool CSingleplayRules::FShouldSwitchWeapon( CBasePlayer *pPlayer, CBaseCombatWeapon *pWeapon )
	{
		//Must have ammo
		if ( ( pWeapon->HasAnyAmmo() == false ) && ( pPlayer->GetAmmoCount( pWeapon->m_iPrimaryAmmoType ) <= 0 ) )
			return false;

		//Always take a loaded gun if we have nothing else
		if ( pPlayer->GetActiveWeapon() == NULL )
			return true;

		// The given weapon must allow autoswitching to it from another weapon.
		if ( !pWeapon->AllowsAutoSwitchTo() )
			return false;

		// The active weapon must allow autoswitching from it.
		if ( !pPlayer->GetActiveWeapon()->AllowsAutoSwitchFrom() )
			return false;

		//Don't switch if our current gun doesn't want to be holstered
		if ( pPlayer->GetActiveWeapon()->CanHolster() == false )
			return false;

		//Only switch if the weapon is better than what we're using
		if ( ( pWeapon != pPlayer->GetActiveWeapon() ) && ( pWeapon->GetWeight() <= pPlayer->GetActiveWeapon()->GetWeight() ) )
			return false;

		return true;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Find the next best weapon to use and return it.
	//-----------------------------------------------------------------------------
	CBaseCombatWeapon *CSingleplayRules::GetNextBestWeapon( CBaseCombatCharacter *pPlayer, CBaseCombatWeapon *pCurrentWeapon )
	{
		if ( pCurrentWeapon && !pCurrentWeapon->AllowsAutoSwitchFrom() )
			return NULL;

		CBaseCombatWeapon	*pBestWeapon = NULL;
		CBaseCombatWeapon	*pWeapon;
		
		int	nBestWeight	= -1;

		//Search for the best weapon to use next based on its weight
		for ( int i = 0; i < pPlayer->WeaponCount(); i++ )
		{
			pWeapon = pPlayer->GetWeapon(i);

			if ( pWeapon == NULL )
				continue;

			// If we have an active weapon and this weapon doesn't allow autoswitching away
			// from another weapon, skip it.
			if ( pCurrentWeapon && !pWeapon->AllowsAutoSwitchTo() )
				continue;

			// Must be eligible for switching to.
			if (!pPlayer->Weapon_CanSwitchTo(pWeapon))
				continue;
			
			// Must be of higher quality.
			if ( pWeapon->GetWeight() <= nBestWeight )
				continue;

			// We must have primary ammo
			if ( pWeapon->UsesClipsForAmmo1() && pWeapon->Clip1() <= 0 && !pPlayer->GetAmmoCount( pWeapon->GetPrimaryAmmoType() ) )
				continue;

			// This is a better candidate than what we had.
			nBestWeight = pWeapon->GetWeight();
			pBestWeapon = pWeapon;
		}

		return pBestWeapon;
	}

	//-----------------------------------------------------------------------------
	// Purpose: 
	// Output : Returns true on success, false on failure.
	//-----------------------------------------------------------------------------
	bool CSingleplayRules::SwitchToNextBestWeapon( CBaseCombatCharacter *pPlayer, CBaseCombatWeapon *pCurrentWeapon )
	{
		CBaseCombatWeapon *pWeapon = GetNextBestWeapon( pPlayer, pCurrentWeapon );

		if ( pWeapon != NULL )
			return pPlayer->Weapon_Switch( pWeapon );

		return false;
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen )
	{
		return true;
	}

	void CSingleplayRules::InitHUD( CBasePlayer *pl )
	{
	}

	//=========================================================
	//=========================================================
	void CSingleplayRules::ClientDisconnected( edict_t *pClient )
	{
	}

	//=========================================================
	//=========================================================
	float CSingleplayRules::FlPlayerFallDamage( CBasePlayer *pPlayer )
	{
		// subtract off the speed at which a player is allowed to fall without being hurt,
		// so damage will be based on speed beyond that, not the entire fall
		//pPlayer->m_Local.m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
		//return pPlayer->m_Local.m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
		return 0;
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::AllowDamage( CBaseEntity *pVictim, const CTakeDamageInfo &info )
	{
		return true;
	}

	//=========================================================
	//=========================================================
	void CSingleplayRules::PlayerSpawn( CBasePlayer *pPlayer )
	{
		// Player no longer gets all weapons to start.
		// He has to pick them up now.  Use impulse 101
		// to give him all weapons
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::AllowAutoTargetCrosshair( void )
	{
		return ( IsSkillLevel(SKILL_EASY) );
	}

	//=========================================================
	//=========================================================
	int	CSingleplayRules::GetAutoAimMode()
	{
		return sk_autoaim_mode.GetInt();
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::FPlayerCanRespawn( CBasePlayer *pPlayer )
	{
		return true;
	}

	//=========================================================
	//=========================================================
	float CSingleplayRules::FlPlayerSpawnTime( CBasePlayer *pPlayer )
	{
		return gpGlobals->curtime;//now!
	}

	//=========================================================
	// IPointsForKill - how many points awarded to anyone
	// that kills this player?
	//=========================================================
	int CSingleplayRules::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
	{
		return 1;
	}

	int CSingleplayRules::IPointsForKillEntity(CBasePlayer *pAttacker, CBaseEntity *pKilled)
	{
		return 1;
	}
	
	//-----------------------------------------------------------------------------
	// Purpose: 
	//-----------------------------------------------------------------------------
	CBasePlayer *CSingleplayRules::GetDeathScorer( CBaseEntity *pKiller, CBaseEntity *pInflictor )
	{
		if ( pKiller)
		{
			if ( pKiller->Classify() == CLASS_PLAYER )
				return (CBasePlayer*)pKiller;

			if (!pKiller->IsNPC())
			{
				// Killing entity might be specifying a scorer player
				IScorer *pScorerInterface = dynamic_cast<IScorer*>(pKiller);
				if (pScorerInterface)
				{
					CBasePlayer *pPlayer = pScorerInterface->GetScorer();
					if (pPlayer)
						return pPlayer;
				}

				// Inflicting entity might be specifying a scoring player
				pScorerInterface = dynamic_cast<IScorer*>(pInflictor);
				if (pScorerInterface)
				{
					CBasePlayer *pPlayer = pScorerInterface->GetScorer();
					if (pPlayer)
						return pPlayer;
				}
			}
		}

		return NULL;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Returns player who should receive credit for kill
	//-----------------------------------------------------------------------------
	CBasePlayer *CSingleplayRules::GetDeathScorer( CBaseEntity *pKiller, CBaseEntity *pInflictor, CBaseEntity *pVictim )
	{
		// if this method not overridden by subclass, just call our default implementation
		return GetDeathScorer( pKiller, pInflictor );
	}

#define KILLING_SPREE_AMOUNT	5
#define KILLING_FRENZY_AMOUNT	10
#define OVERKILL_AMOUNT	15
#define RAMPAGE_AMOUNT	20
#define UNSTOPPABLE_AMOUNT	25
#define INCONCEIVABLE_AMOUNT	30
#define INVINCIBLE_AMOUNT	35
#define GODLIKE_AMOUNT	40

	//=========================================================
	// PlayerKilled - someone/something killed this player
	//=========================================================
	void CSingleplayRules::PlayerKilled( CBasePlayer *pVictim, const CTakeDamageInfo &info )
	{
		//DeathNotice( pVictim, info );

			// Find the killer & the scorer
		CBaseEntity* pInflictor = info.GetInflictor();
		CBaseEntity* pKiller = info.GetAttacker();
		CBasePlayer* pScorer = GetDeathScorer(pKiller, pInflictor, pVictim);

		pVictim->IncrementDeathCount(1);

		// dvsents2: uncomment when removing all FireTargets
		// variant_t value;
		// g_EventQueue.AddEvent( "game_playerdie", "Use", value, 0, pVictim, pVictim );
		FireTargets("game_playerdie", pVictim, pVictim, USE_TOGGLE, 0);

		// Did the player kill himself?
		if (pVictim == pScorer)
		{
			// Players lose a frag for letting the world kill them			
			pVictim->IncrementFragCount(-1);
		}
		else if (pScorer)
		{
			if (!pScorer->IsNPC())
			{
				if (g_fr_economy.GetBool())
				{
					pScorer->AddMoney(10);
				}
				if (!g_fr_classic.GetBool())
				{
					pScorer->AddXP(15);
				}

				// if a player dies in a deathmatch game and the killer is a client, award the killer some points
				pScorer->IncrementFragCount(IPointsForKill(pScorer, pVictim));

				if (sv_player_voice.GetBool())
				{
					if (sv_player_voice_kill.GetBool())
					{
						int killvoicerandom = random->RandomInt(0, sv_player_voice_kill_freq.GetInt());
						if (killvoicerandom == 0)
						{
							pScorer->EmitSound("Player.VoiceKill");
						}
					}
				}

				if (sv_killingspree.GetBool())
				{
					int m_iKillsInSpree = pScorer->FragCount();

					if (m_iKillsInSpree == KILLING_SPREE_AMOUNT)
					{
						CFmtStr hint;
						hint.sprintf("#Valve_Hud_KILLINGSPREE");
						pScorer->ShowLevelMessage(hint.Access());
						if (g_fr_economy.GetBool())
						{
							pScorer->AddMoney(2);
						}
						if (!g_fr_classic.GetBool())
						{
							pScorer->AddXP(3);
						}
					}
					if (m_iKillsInSpree == KILLING_FRENZY_AMOUNT)
					{
						CFmtStr hint;
						hint.sprintf("#Valve_Hud_KILLINGFRENZY");
						pScorer->ShowLevelMessage(hint.Access());
						if (g_fr_economy.GetBool())
						{
							pScorer->AddMoney(4);
						}
						if (!g_fr_classic.GetBool())
						{
							pScorer->AddXP(6);
						}
					}
					if (m_iKillsInSpree == OVERKILL_AMOUNT)
					{
						CFmtStr hint;
						hint.sprintf("#Valve_Hud_OVERKILL");
						pScorer->ShowLevelMessage(hint.Access());
						if (g_fr_economy.GetBool())
						{
							pScorer->AddMoney(6);
						}
						if (!g_fr_classic.GetBool())
						{
							pScorer->AddXP(9);
						}
					}
					if (m_iKillsInSpree == RAMPAGE_AMOUNT)
					{
						CFmtStr hint;
						hint.sprintf("#Valve_Hud_RAMPAGE");
						pScorer->ShowLevelMessage(hint.Access());
						if (g_fr_economy.GetBool())
						{
							pScorer->AddMoney(8);
						}
						if (!g_fr_classic.GetBool())
						{
							pScorer->AddXP(12);
						}
					}
					if (m_iKillsInSpree == UNSTOPPABLE_AMOUNT)
					{
						CFmtStr hint;
						hint.sprintf("#Valve_Hud_UNSTOPPABLE");
						pScorer->ShowLevelMessage(hint.Access());
						if (g_fr_economy.GetBool())
						{
							pScorer->AddMoney(10);
						}
						if (!g_fr_classic.GetBool())
						{
							pScorer->AddXP(15);
						}
					}
					if (m_iKillsInSpree == INCONCEIVABLE_AMOUNT)
					{
						CFmtStr hint;
						hint.sprintf("#Valve_Hud_INCONCEIVABLE");
						pScorer->ShowLevelMessage(hint.Access());
						if (g_fr_economy.GetBool())
						{
							pScorer->AddMoney(12);
						}
						if (!g_fr_classic.GetBool())
						{
							pScorer->AddXP(18);
						}
					}
					if (m_iKillsInSpree == INVINCIBLE_AMOUNT)
					{
						CFmtStr hint;
						hint.sprintf("#Valve_Hud_INVINCIBLE");
						pScorer->ShowLevelMessage(hint.Access());
						if (g_fr_economy.GetBool())
						{
							pScorer->AddMoney(14);
						}
						if (!g_fr_classic.GetBool())
						{
							pScorer->AddXP(21);
						}
					}
					if (m_iKillsInSpree == GODLIKE_AMOUNT)
					{
						CFmtStr hint;
						hint.sprintf("#Valve_Hud_GODLIKE");
						pScorer->ShowLevelMessage(hint.Access());
						if (g_fr_economy.GetBool())
						{
							pScorer->AddMoney(16);
						}
						if (!g_fr_classic.GetBool())
						{
							pScorer->AddXP(24);
						}
					}

#define CLASSICLEVELUP_AMOUNT	15

					if (g_fr_classic.GetBool())
					{
						int m_iKillsInClassicMode = 0;

						m_iKillsInClassicMode++;

						if (m_iKillsInClassicMode == CLASSICLEVELUP_AMOUNT)
						{
							pScorer->LevelUpClassic();
							m_iKillsInClassicMode = 0;
						}
					}
				}
			}

			// dvsents2: uncomment when removing all FireTargets
			//variant_t value;
			//g_EventQueue.AddEvent( "game_playerkill", "Use", value, 0, pScorer, pScorer );
			FireTargets("game_playerkill", pScorer, pScorer, USE_TOGGLE, 0);
		}
		else
		{
			// Players lose a frag for letting the world kill them			
			pVictim->IncrementFragCount(-1);
		}
	}

	void CSingleplayRules::NPCKilled(CBaseEntity *pVictim, const CTakeDamageInfo &info)
	{
		// Find the killer & the scorer
		CBaseEntity *pInflictor = info.GetInflictor();
		CBaseEntity *pKiller = info.GetAttacker();
		CBasePlayer *pEntity = GetDeathScorer(pKiller, pInflictor, pVictim);
		
		if (pEntity)
		{
			if (pVictim->m_isRareEntity)
			{
				switch (GetSkillLevel())
				{
				case SKILL_EASY:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(3 * sk_money_multiplier1.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(5 * sk_exp_multiplier1.GetInt());
					}
					break;

				case SKILL_MEDIUM:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(3 * sk_money_multiplier2.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(5 * sk_exp_multiplier2.GetInt());
					}
					break;

				case SKILL_HARD:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(3 * sk_money_multiplier3.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(5 * sk_exp_multiplier3.GetInt());
					}
					break;

				case SKILL_VERYHARD:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(3 * sk_money_multiplier4.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(5 * sk_exp_multiplier4.GetInt());
					}
					break;

				case SKILL_NIGHTMARE:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(3 * sk_money_multiplier5.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(5 * sk_exp_multiplier5.GetInt());
					}
					break;
				}
			}
			else
			{
				switch (GetSkillLevel())
				{
				case SKILL_EASY:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(2 * sk_money_multiplier1.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(3 * sk_exp_multiplier1.GetInt());
					}
					break;

				case SKILL_MEDIUM:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(2 * sk_money_multiplier2.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(3 * sk_exp_multiplier2.GetInt());
					}
					break;

				case SKILL_HARD:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(2 * sk_money_multiplier3.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(3 * sk_exp_multiplier3.GetInt());
					}
					break;

				case SKILL_VERYHARD:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(2 * sk_money_multiplier4.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(3 * sk_exp_multiplier4.GetInt());
					}
					break;

				case SKILL_NIGHTMARE:
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(2 * sk_money_multiplier5.GetInt());
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(3 * sk_exp_multiplier5.GetInt());
					}
					break;
				}
			}

			pEntity->IncrementFragCount(IPointsForKillEntity(pEntity, pVictim));

			if (info.GetInflictor() == pEntity)
			{
				if (sv_player_voice.GetBool())
				{
					if (sv_player_voice_kill.GetBool())
					{
						int killvoicerandom = random->RandomInt(0, sv_player_voice_kill_freq.GetInt());
						if (killvoicerandom == 0)
						{
							pEntity->EmitSound("Player.VoiceKill");
						}
					}
				}
			}

			if (sv_killingspree.GetBool())
			{
				int m_iKillsInSpree = pEntity->FragCount();

				if (m_iKillsInSpree == KILLING_SPREE_AMOUNT)
				{
					CFmtStr hint;
					hint.sprintf("#Valve_Hud_KILLINGSPREE");
					pEntity->ShowLevelMessage(hint.Access());
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(2);
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(3);
					}
				}
				if (m_iKillsInSpree == KILLING_FRENZY_AMOUNT)
				{
					CFmtStr hint;
					hint.sprintf("#Valve_Hud_KILLINGFRENZY");
					pEntity->ShowLevelMessage(hint.Access());
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(4);
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(6);
					}
				}
				if (m_iKillsInSpree == OVERKILL_AMOUNT)
				{
					CFmtStr hint;
					hint.sprintf("#Valve_Hud_OVERKILL");
					pEntity->ShowLevelMessage(hint.Access());
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(6);
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(9);
					}
				}
				if (m_iKillsInSpree == RAMPAGE_AMOUNT)
				{
					CFmtStr hint;
					hint.sprintf("#Valve_Hud_RAMPAGE");
					pEntity->ShowLevelMessage(hint.Access());
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(8);
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(12);
					}
				}
				if (m_iKillsInSpree == UNSTOPPABLE_AMOUNT)
				{
					CFmtStr hint;
					hint.sprintf("#Valve_Hud_UNSTOPPABLE");
					pEntity->ShowLevelMessage(hint.Access());
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(10);
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(15);
					}
				}
				if (m_iKillsInSpree == INCONCEIVABLE_AMOUNT)
				{
					CFmtStr hint;
					hint.sprintf("#Valve_Hud_INCONCEIVABLE");
					pEntity->ShowLevelMessage(hint.Access());
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(12);
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(18);
					}
				}
				if (m_iKillsInSpree == INVINCIBLE_AMOUNT)
				{
					CFmtStr hint;
					hint.sprintf("#Valve_Hud_INVINCIBLE");
					pEntity->ShowLevelMessage(hint.Access());
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(14);
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(21);
					}
				}
				if (m_iKillsInSpree == GODLIKE_AMOUNT)
				{
					CFmtStr hint;
					hint.sprintf("#Valve_Hud_GODLIKE");
					pEntity->ShowLevelMessage(hint.Access());
					if (g_fr_economy.GetBool())
					{
						pEntity->AddMoney(16);
					}
					if (!g_fr_classic.GetBool())
					{
						pEntity->AddXP(24);
					}
				}
			}

			if (pVictim->m_isRareEntity)
			{
				if (g_fr_classic.GetBool())
				{
					pEntity->LevelUpClassic();
				}
			}
		}
	}

	//=========================================================
	// Deathnotice
	//=========================================================
	void CSingleplayRules::DeathNotice( CBasePlayer *pVictim, const CTakeDamageInfo &info )
	{
	}

	//=========================================================
	// FlWeaponRespawnTime - what is the time in the future
	// at which this weapon may spawn?
	//=========================================================
	float CSingleplayRules::FlWeaponRespawnTime( CBaseCombatWeapon *pWeapon )
	{
		if (sv_weapon_respawn.GetBool())
		{
			return sv_weapon_respawn_time.GetFloat();
		}
		else
		{
		return -1;
	}
	}

	#define ENTITY_INTOLERANCE	100
	//=========================================================
	// FlWeaponRespawnTime - Returns 0 if the weapon can respawn 
	// now,  otherwise it returns the time at which it can try
	// to spawn again.
	//=========================================================
	float CSingleplayRules::FlWeaponTryRespawn( CBaseCombatWeapon *pWeapon )
	{
		if ( gEntList.NumberOfEntities() < (gpGlobals->maxEntities - ENTITY_INTOLERANCE) )
		return 0;

		// we're past the entity tolerance level,  so delay the respawn
		return FlWeaponRespawnTime( pWeapon );
	}

	//=========================================================
	// VecWeaponRespawnSpot - where should this weapon spawn?
	// Some game variations may choose to randomize spawn locations
	//=========================================================
	Vector CSingleplayRules::VecWeaponRespawnSpot( CBaseCombatWeapon *pWeapon )
	{
		return pWeapon->GetAbsOrigin();
	}

	//=========================================================
	// WeaponShouldRespawn - any conditions inhibiting the
	// respawning of this weapon?
	//=========================================================
	int CSingleplayRules::WeaponShouldRespawn( CBaseCombatWeapon *pWeapon )
	{
		if (sv_weapon_respawn.GetBool())
		{
			if (pWeapon->HasSpawnFlags(SF_NORESPAWN))
			{
		return GR_WEAPON_RESPAWN_NO;
	}
			else
			{
				return GR_WEAPON_RESPAWN_YES;
			}
		}
		else
		{
			return GR_WEAPON_RESPAWN_NO;
		}
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::CanHaveItem( CBasePlayer *pPlayer, CItem *pItem )
	{
		return true;
	}

	//=========================================================
	//=========================================================
	void CSingleplayRules::PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem )
	{
	}

	//=========================================================
	//=========================================================
	int CSingleplayRules::ItemShouldRespawn( CItem *pItem )
	{
		if (sv_item_respawn.GetBool())
		{
			if (pItem->HasSpawnFlags(SF_NORESPAWN))
			{
				return GR_ITEM_RESPAWN_NO;
			}
			else
			{
				return GR_ITEM_RESPAWN_YES;
			}
		}
		else
		{
		return GR_ITEM_RESPAWN_NO;
	}
	}


	//=========================================================
	// At what time in the future may this Item respawn?
	//=========================================================
	float CSingleplayRules::FlItemRespawnTime( CItem *pItem )
	{
		if (sv_item_respawn.GetBool())
		{
			return sv_item_respawn_time.GetFloat();
		}
		else
		{
		return -1;
	}
	}

	//=========================================================
	// Where should this item respawn?
	// Some game variations may choose to randomize spawn locations
	//=========================================================
	Vector CSingleplayRules::VecItemRespawnSpot( CItem *pItem )
	{
		return pItem->GetAbsOrigin();
	}

	//=========================================================
	// What angles should this item use to respawn?
	//=========================================================
	QAngle CSingleplayRules::VecItemRespawnAngles( CItem *pItem )
	{
		return pItem->GetAbsAngles();
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::IsAllowedToSpawn( CBaseEntity *pEntity )
	{
		return true;
	}

	//=========================================================
	//=========================================================
	void CSingleplayRules::PlayerGotAmmo( CBaseCombatCharacter *pPlayer, char *szName, int iCount )
	{
	}

	//=========================================================
	//=========================================================
	float CSingleplayRules::FlHealthChargerRechargeTime( void )
	{
		if (sv_healthcharger_recharge.GetBool())
		{
			return sv_healthcharger_recharge_time.GetFloat();
		}
		else
		{
			return 0;// don't recharge
		}
	}

	float CSingleplayRules::FlHEVChargerRechargeTime(void)
	{
		if (sv_suitcharger_recharge.GetBool())
		{
			return sv_suitcharger_recharge_time.GetFloat();
		}
		else
		{
			return 0;// don't recharge
		}
	}

	//=========================================================
	//=========================================================
	int CSingleplayRules::DeadPlayerWeapons( CBasePlayer *pPlayer )
	{
		if (sv_player_dropweaponsondeath.GetBool())
		{
			return GR_PLR_DROP_GUN_ACTIVE;
		}
		else
		{
			return GR_PLR_DROP_GUN_NO;
		}
	}

	//=========================================================
	//=========================================================
	int CSingleplayRules::DeadPlayerAmmo( CBasePlayer *pPlayer )
	{
		if (sv_player_dropweaponsondeath.GetBool())
		{
			return GR_PLR_DROP_AMMO_ACTIVE;
		}
		else
		{
			return GR_PLR_DROP_AMMO_NO;
		}
	}

	//=========================================================
	//=========================================================
	int CSingleplayRules::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
	{
		// why would a single player in half life need this? good question.
		return GR_TEAMMATE;
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::PlayerCanHearChat( CBasePlayer *pListener, CBasePlayer *pSpeaker )
	{
		return true;
	}

	//=========================================================
	//=========================================================
	bool CSingleplayRules::FAllowNPCs( void )
	{
		return true;
	}

#endif

