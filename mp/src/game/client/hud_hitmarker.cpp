//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Hit Marker
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_hitmarker.h"
#include "iclientmode.h"
#include "c_baseplayer.h"
#include "engine/ienginesound.h"
#include "gamevars_shared.h"

// VGUI panel includes
#include <vgui_controls/AnimationController.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>

using namespace vgui;
 
// memdbgon must be the last include file in a .cpp file!
#include "tier0/memdbgon.h"
 
DECLARE_HUDELEMENT( CHudHitmarker );

ConVar la_cl_hitmarkers("la_cl_hitmarkers", "1", FCVAR_ARCHIVE, "Enable hitmarkers (client option)");

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
 
CHudHitmarker::CHudHitmarker( const char *pElementName ) : CHudElement(pElementName), BaseClass(NULL, "HudHitmarker")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
 
	// Hitmarker will not show when the player is dead
	SetHiddenBits( HIDEHUD_PLAYERDEAD );
 
	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
}
 
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHitmarker::Init()
{
	SetAlpha( 0 );

	enginesound->PrecacheSound("ui/hitsounds/ag2_hitbody.wav");
	enginesound->PrecacheSound("ui/hitsounds/ag2_hithead.wav");

	ListenForGameEvent("player_hurt");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHitmarker::Reset()
{
	SetAlpha( 0 );
}
 
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHitmarker::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings(scheme);
 
	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudHitmarker::ShouldDraw( void )
{
	return ( CHudElement::ShouldDraw() );
}
 
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHitmarker::Paint( void )
{
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (!pPlayer)
	{
		return;
	}
 
	int		x,y;

	// Find our screen position to start from
	x = XRES(320);
	y = YRES(240);

	vgui::surface()->DrawSetColor( m_HitmarkerColor );
	vgui::surface()->DrawLine( x - 6, y - 5, x - 11, y - 10 );
	vgui::surface()->DrawLine( x + 5, y - 5, x + 10, y - 10 );
	vgui::surface()->DrawLine( x - 6, y + 5, x - 11, y + 10 );
	vgui::surface()->DrawLine( x + 5, y + 5, x + 10, y + 10 );
}

void CHudHitmarker::FireGameEvent(IGameEvent *event)
{
	if (!la_sv_hitmarkers.GetBool() || !la_cl_hitmarkers.GetBool())
		return;

	const char * type = event->GetName();

	if (Q_strcmp(type, "player_hurt") == 0)
	{
		C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
		int iAttacker = event->GetInt("attacker");
		bool bHeadshot = event->GetBool("headshot");

		if (iAttacker != pPlayer->GetUserID())
			return;

		CLocalPlayerFilter filter;

		if (bHeadshot)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("HitMarkerShowHeadshot");
			enginesound->EmitSound(filter, -1, CHAN_AUTO, "ui/hitsounds/ag2_hithead.wav", 1.0, SNDLVL_NORM, 0);
		}
		else
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("HitMarkerShow");
			enginesound->EmitSound(filter, -1, CHAN_AUTO, "ui/hitsounds/ag2_hitbody.wav", 1.0, SNDLVL_NORM, 0);
		}
	}
}