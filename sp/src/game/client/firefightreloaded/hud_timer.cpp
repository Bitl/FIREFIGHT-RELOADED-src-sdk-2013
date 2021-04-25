//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
//
// Health.cpp
//
// implementation of CHudTimer class
//
#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "view.h"

#include "iclientmode.h"

#include <KeyValues.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>

#include <vgui/ILocalize.h>

#include <vgui/IVGui.h>

using namespace vgui;

#include "hudelement.h"
#include "hud_basetimer.h"

#include "convar.h"

#include "engine/IEngineSound.h"

#include "hl2/hl2_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define INIT_TIMER -1

ConVar cl_fr_usetimer("cl_fr_usetimer", "1", FCVAR_ARCHIVE);

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CHudTimer : public CHudElement, public CHudBaseTimer
{
	DECLARE_CLASS_SIMPLE( CHudTimer, CHudBaseTimer );

public:
	CHudTimer( const char *pElementName );
	virtual void Init( void );
	virtual void VidInit( void );
	virtual void Reset( void );
	virtual void OnThink();

private:
};	

DECLARE_HUDELEMENT( CHudTimer );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudTimer::CHudTimer( const char *pElementName ) : CHudElement( pElementName ), CHudBaseTimer(NULL, "HudTimer")
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudTimer::Init()
{
	Reset();
	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("TimerInit");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudTimer::Reset()
{
	SetMinutes(INIT_TIMER);
	SetSeconds(INIT_TIMER);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudTimer::VidInit()
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudTimer::OnThink()
{
	if (g_pGameRules && !g_pGameRules->IsMultiplayer() && cl_fr_usetimer.GetBool())
	{
		int iRemain = (int)gpGlobals->curtime;
		int iMinutes, iSeconds;
		iMinutes = iRemain / 60;
		iSeconds = iRemain % 60;
		SetMinutes(iMinutes);
		SetSeconds(iSeconds);
		SetAlpha(255);
	}
	else if (g_pGameRules && g_pGameRules->IsMultiplayer() && HL2GameRules()->GetMapRemainingTime() > 0)
	{
		SetAlpha(255);
		int iRemain = (int)HL2GameRules()->GetMapRemainingTime();
		int iMinutes, iSeconds;
		iMinutes = iRemain / 60;
		iSeconds = iRemain % 60;
		SetMinutes(iMinutes);
		SetSeconds(iSeconds);

		if (iMinutes == 0 && iSeconds < 30)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("TimerBelow30");
		}
		else
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("TimerAbove30");
		}
	}
	else
	{
		SetAlpha(0);
	}
}