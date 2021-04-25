//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef MONSTERMAKER_FIREFIGHT_H
#define MONSTERMAKER_FIREFIGHT_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"


//-----------------------------------------------------------------------------
// Spawnflags
//-----------------------------------------------------------------------------
#define SF_NPCMAKER_FADE			1	// Children's corpses fade
//TODO: add mutators, including this.
#define SF_NPCMAKER_DOUBLETROUBLE	8	//Spawn double the enemies
//#define SF_NPCMAKER_ALWAYSUSERADIUS	32	// Use radius spawn whenever spawning

class CNPCMakerFirefight : public CBaseEntity
{
public:
	DECLARE_CLASS(CNPCMakerFirefight, CBaseEntity);

	CNPCMakerFirefight(void);

	void Spawn( void );
	void Precache(void);
	virtual int	ObjectCaps( void ) { return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	void MakerThink( void );
	bool HumanHullFits( const Vector &vecLocation );
	bool CanMakeNPC( bool bIgnoreSolidEntities = false );
	bool CanMakeRareNPC();

	virtual void DeathNotice( CBaseEntity *pChild );// NPC maker children use this to tell the NPC maker that they have died.
	virtual void MakeNPC(bool rareNPC = false);

	virtual	void ChildPreSpawn( CAI_BaseNPC *pChild ) {};
	virtual	void ChildPostSpawn( CAI_BaseNPC *pChild );

	// Input handlers
	void InputSpawnNPC( inputdata_t &inputdata );
	void InputSpawnRareNPC(inputdata_t &inputdata);
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputToggle( inputdata_t &inputdata );
	void InputSetMaxLiveChildren( inputdata_t &inputdata );
	void InputSetMaxLiveRareNPCS( inputdata_t &inputdata );
	void InputSetSpawnFrequency( inputdata_t &inputdata );
	void InputSetRareNPCRarity(inputdata_t &inputdata);

	// State changers
	void Toggle( void );
	virtual void Enable( void );
	virtual void Disable( void );

	virtual bool IsDepleted( void );

	DECLARE_DATADESC();
	
	float		m_flSpawnFrequency;		// delay (in secs) between spawns
	int			m_nRareNPCRarity;		// Sets the rarity of rare NPCs

	COutputEHANDLE m_OnSpawnNPC;
	COutputEHANDLE m_OnSpawnRareNPC;
	COutputEvent m_OnAllLiveChildrenDead;
	
	int		m_nLiveChildren;	// how many NPCs made by this NPC maker that are currently alive
	int		m_nMaxLiveChildren;	// max number of NPCs that this maker may have out at one time.
	int		m_nLiveRareNPCs;
	int		m_nMaxLiveRareNPCs;

	bool	m_bDisabled;

	EHANDLE m_hIgnoreEntity;
	string_t m_iszIngoreEnt;

	string_t m_iszNPCClassname;			// classname of the NPC(s) that will be created.
	string_t m_SquadName;
	string_t m_strHintGroup;
	string_t m_spawnEquipment;
	string_t m_RelationshipString;		// Used to load up relationship keyvalues
	string_t m_ChildTargetName;
};

#endif // MONSTERMAKER_H
