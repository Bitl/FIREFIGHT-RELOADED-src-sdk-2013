//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The various ammo types for HL2	
//
//=============================================================================//

#include "cbase.h"
#include "props.h"
#include "items.h"
#include "item_dynamic_resupply.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// A breakable crate that drops items
//-----------------------------------------------------------------------------
class CItem_ItemCrateFirefight : public CPhysicsProp
{
public:
	DECLARE_CLASS( CItem_ItemCrateFirefight, CPhysicsProp );
	DECLARE_DATADESC();

	void Precache( void );
	void Spawn( void );

	virtual int	ObjectCaps() { return BaseClass::ObjectCaps() | FCAP_WCEDIT_POSITION; };

	virtual int		OnTakeDamage( const CTakeDamageInfo &info );

	void InputKill( inputdata_t &data );

	virtual void VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );
	virtual void OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason );

protected:
	virtual void OnBreak( const Vector &vecVelocity, const AngularImpulse &angVel, CBaseEntity *pBreaker );

private:
	string_t			m_strItemClass1;
	string_t			m_strItemClass2;
	string_t			m_strItemClass3;
	string_t			m_strItemClass4;
	string_t			m_strItemClass5;
	string_t			m_strItemClass6;
	string_t			m_strItemClass7;
	string_t			m_strItemClass8;
	string_t			m_strItemClass9;
	string_t			m_strItemClass10;
	int					m_nItemCount;
	string_t			m_strAlternateMaster;
	string_t			m_CrateAppearance;

	COutputEvent m_OnCacheInteraction;
};


LINK_ENTITY_TO_CLASS(item_crate_firefight, CItem_ItemCrateFirefight);


//-----------------------------------------------------------------------------
// Save/load: 
//-----------------------------------------------------------------------------
BEGIN_DATADESC( CItem_ItemCrateFirefight )
	
	DEFINE_KEYFIELD( m_strItemClass1, FIELD_STRING, "ItemClass1" ),
	DEFINE_KEYFIELD(m_strItemClass2, FIELD_STRING, "ItemClass2"),
	DEFINE_KEYFIELD(m_strItemClass3, FIELD_STRING, "ItemClass3"),
	DEFINE_KEYFIELD(m_strItemClass4, FIELD_STRING, "ItemClass4"),
	DEFINE_KEYFIELD(m_strItemClass5, FIELD_STRING, "ItemClass5"),
	DEFINE_KEYFIELD(m_strItemClass6, FIELD_STRING, "ItemClass6"),
	DEFINE_KEYFIELD(m_strItemClass7, FIELD_STRING, "ItemClass7"),
	DEFINE_KEYFIELD(m_strItemClass8, FIELD_STRING, "ItemClass8"),
	DEFINE_KEYFIELD(m_strItemClass9, FIELD_STRING, "ItemClass9"),
	DEFINE_KEYFIELD(m_strItemClass10, FIELD_STRING, "ItemClass10"),
	DEFINE_KEYFIELD( m_nItemCount, FIELD_INTEGER, "ItemCount" ),	
	DEFINE_KEYFIELD( m_strAlternateMaster, FIELD_STRING, "SpecificResupply" ),	
	DEFINE_KEYFIELD( m_CrateAppearance, FIELD_STRING, "CrateAppearance" ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Kill", InputKill ),
	DEFINE_OUTPUT( m_OnCacheInteraction, "OnCacheInteraction" ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItem_ItemCrateFirefight::Precache( void )
{
	// Set this here to quiet base prop warnings
	PrecacheModel(STRING(m_CrateAppearance));
	SetModel(STRING(m_CrateAppearance));

	BaseClass::Precache();

	if (NULL_STRING != m_strItemClass1)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass1));
	}

	if (NULL_STRING != m_strItemClass2)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass2));
	}

	if (NULL_STRING != m_strItemClass3)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass3));
	}

	if (NULL_STRING != m_strItemClass4)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass4));
	}

	if (NULL_STRING != m_strItemClass5)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass5));
	}

	if (NULL_STRING != m_strItemClass6)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass6));
	}

	if (NULL_STRING != m_strItemClass7)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass7));
	}

	if (NULL_STRING != m_strItemClass8)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass8));
	}

	if (NULL_STRING != m_strItemClass9)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass9));
	}

	if (NULL_STRING != m_strItemClass10)
	{
		// Don't precache if this is a null string. 
		UTIL_PrecacheOther(STRING(m_strItemClass10));
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItem_ItemCrateFirefight::Spawn( void )
{ 
	if ( g_pGameRules->IsAllowedToSpawn( this ) == false )
	{
		UTIL_Remove( this );
		return;
	}

	if (NULL_STRING == m_CrateAppearance)
	{
		Warning("CItem_ItemCrateFirefight(%i):  Model not found. Have you defined the model in your map/mapadd? (deleted)\n", entindex());
		UTIL_Remove(this);
		return;
	}

	DisableAutoFade();
	SetModelName(AllocPooledString(STRING(m_CrateAppearance)));

	if ( NULL_STRING == m_strItemClass1 )
	{
		Warning( "CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass1 string (deleted)!!!\n", entindex() );
		UTIL_Remove( this );
		return;
	}

	if (NULL_STRING == m_strItemClass2)
	{
		Warning("CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass2 string (deleted)!!!\n", entindex());
		UTIL_Remove(this);
		return;
	}

	if (NULL_STRING == m_strItemClass3)
	{
		Warning("CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass3 string (deleted)!!!\n", entindex());
		UTIL_Remove(this);
		return;
	}

	if (NULL_STRING == m_strItemClass4)
	{
		Warning("CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass4 string (deleted)!!!\n", entindex());
		UTIL_Remove(this);
		return;
	}

	if (NULL_STRING == m_strItemClass5)
	{
		Warning("CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass5 string (deleted)!!!\n", entindex());
		UTIL_Remove(this);
		return;
	}

	if (NULL_STRING == m_strItemClass6)
	{
		Warning("CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass6 string (deleted)!!!\n", entindex());
		UTIL_Remove(this);
		return;
	}

	if (NULL_STRING == m_strItemClass7)
	{
		Warning("CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass7 string (deleted)!!!\n", entindex());
		UTIL_Remove(this);
		return;
	}

	if (NULL_STRING == m_strItemClass8)
	{
		Warning("CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass8 string (deleted)!!!\n", entindex());
		UTIL_Remove(this);
		return;
	}

	if (NULL_STRING == m_strItemClass9)
	{
		Warning("CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass9 string (deleted)!!!\n", entindex());
		UTIL_Remove(this);
		return;
	}

	if (NULL_STRING == m_strItemClass10)
	{
		Warning("CItem_ItemCrateFirefight(%i):  CRATE_SPECIFIC_ITEM with NULL ItemClass10 string (deleted)!!!\n", entindex());
		UTIL_Remove(this);
		return;
	}

	Precache( );
	SetModel(STRING(m_CrateAppearance));
	AddEFlags( EFL_NO_ROTORWASH_PUSH );
	BaseClass::Spawn( );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &data - 
//-----------------------------------------------------------------------------
void CItem_ItemCrateFirefight::InputKill( inputdata_t &data )
{
	UTIL_Remove( this );
}


//-----------------------------------------------------------------------------
// Item crates blow up immediately
//-----------------------------------------------------------------------------
int CItem_ItemCrateFirefight::OnTakeDamage( const CTakeDamageInfo &info )
{
	if ( info.GetDamageType() & DMG_AIRBOAT || info.GetAttacker()->IsPlayer() && info.GetDamageType() & (DMG_CRUSH | DMG_SLASH | DMG_CLUB) )
	{
		CTakeDamageInfo dmgInfo = info;
		dmgInfo.ScaleDamage( 10.0 );
		return BaseClass::OnTakeDamage( dmgInfo );
	}

	return BaseClass::OnTakeDamage( info );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItem_ItemCrateFirefight::VPhysicsCollision( int index, gamevcollisionevent_t *pEvent )
{
	float flDamageScale = 1.0f;
	if ( FClassnameIs( pEvent->pEntities[!index], "prop_vehicle_airboat" ) ||
		 FClassnameIs( pEvent->pEntities[!index], "prop_vehicle_jeep" ) )
	{
		flDamageScale = 100.0f;
	}

	m_impactEnergyScale *= flDamageScale;
	BaseClass::VPhysicsCollision( index, pEvent );
	m_impactEnergyScale /= flDamageScale;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItem_ItemCrateFirefight::OnBreak( const Vector &vecVelocity, const AngularImpulse &angImpulse, CBaseEntity *pBreaker )
{
	// FIXME: We could simply store the name of an entity to put into the crate 
	// as a string entered in by worldcraft. Should we?	I'd do it for sure
	// if it was easy to get a dropdown with all entity types in it.

	string_t g_charItemSpawnList[] =
	{
		m_strItemClass1,
		m_strItemClass2,
		m_strItemClass3,
		m_strItemClass4,
		m_strItemClass5,
		m_strItemClass6,
		m_strItemClass7,
		m_strItemClass8,
		m_strItemClass9,
		m_strItemClass10,
	};

	m_OnCacheInteraction.FireOutput(pBreaker,this);

	for ( int i = 0; i < m_nItemCount; ++i )
	{
		CBaseEntity *pSpawn = NULL;
		int nItems = ARRAYSIZE(g_charItemSpawnList);
		int randomChoice = rand() % nItems;
		string_t pRandomName = g_charItemSpawnList[randomChoice];
		pSpawn = CreateEntityByName(STRING(pRandomName));

		if ( !pSpawn )
			return;

		// Give a little randomness...
		Vector vecOrigin;
		CollisionProp()->RandomPointInBounds( Vector(0.25, 0.25, 0.25), Vector( 0.75, 0.75, 0.75 ), &vecOrigin );
		pSpawn->SetAbsOrigin( vecOrigin );

		QAngle vecAngles;
		vecAngles.x = random->RandomFloat( -20.0f, 20.0f );
		vecAngles.y = random->RandomFloat( 0.0f, 360.0f );
		vecAngles.z = random->RandomFloat( -20.0f, 20.0f );
		pSpawn->SetAbsAngles( vecAngles );

		Vector vecActualVelocity;
		vecActualVelocity.Random( -10.0f, 10.0f );
//		vecActualVelocity += vecVelocity;
		pSpawn->SetAbsVelocity( vecActualVelocity );

		QAngle angVel;
		AngularImpulseToQAngle( angImpulse, angVel );
		pSpawn->SetLocalAngularVelocity( angVel );

		// If we're creating an item, it can't be picked up until it comes to rest
		// But only if it wasn't broken by a vehicle
		CItem *pItem = dynamic_cast<CItem*>(pSpawn);
		if ( pItem && !pBreaker->GetServerVehicle())
		{
			pItem->ActivateWhenAtRest();
		}

		pSpawn->Spawn();

		// Avoid missing items drops by a dynamic resupply because they don't think immediately
		if ( FClassnameIs( pSpawn, "item_dynamic_resupply" ) )
		{
			if ( m_strAlternateMaster != NULL_STRING )
			{
				DynamicResupply_InitFromAlternateMaster( pSpawn, m_strAlternateMaster );
			}
			if ( i == 0 )
			{
				pSpawn->AddSpawnFlags( SF_DYNAMICRESUPPLY_ALWAYS_SPAWN );
			}
			pSpawn->SetNextThink( gpGlobals->curtime );
		}
	}
}

void CItem_ItemCrateFirefight::OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason )
{
	BaseClass::OnPhysGunPickup( pPhysGunUser, reason );

	m_OnCacheInteraction.FireOutput( pPhysGunUser, this );

	if (reason == PUNTED_BY_CANNON)
	{
		Vector vForward;
		AngleVectors( pPhysGunUser->EyeAngles(), &vForward, NULL, NULL );
		Vector vForce = Pickup_PhysGunLaunchVelocity( this, vForward, PHYSGUN_FORCE_PUNTED );
		AngularImpulse angular = AngularImpulse( 0, 0, 0 );

		IPhysicsObject *pPhysics = VPhysicsGetObject();

		if ( pPhysics )
		{
			pPhysics->AddVelocity( &vForce, &angular );
		}

		TakeDamage( CTakeDamageInfo( pPhysGunUser, pPhysGunUser, GetHealth(), DMG_GENERIC ) );
	}
}
