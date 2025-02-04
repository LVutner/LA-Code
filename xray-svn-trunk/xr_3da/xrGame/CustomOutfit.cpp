#include "stdafx.h"

#include "customoutfit.h"
#include "PhysicsShell.h"
#include "inventory_space.h"
#include "Inventory.h"
#include "Actor.h"
#include "game_cl_base.h"
#include "Level.h"
#include "BoneProtections.h"
#include "../../Include/xrRender/Kinematics.h"
#include "../../Include/xrRender/RenderVisual.h"
#include "ai_sounds.h"
#include "actorEffector.h"

CCustomOutfit::CCustomOutfit()
{
	m_slot = OUTFIT_SLOT;

	m_flags.set(FUsingCondition, TRUE);

	m_HitTypeProtection.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeProtection[i] = 1.0f;

	m_boneProtection = xr_new<SBoneProtections>();
}

CCustomOutfit::~CCustomOutfit() 
{
	xr_delete(m_boneProtection);

	HUD_SOUND::DestroySound	(m_NightVisionOnSnd);
	HUD_SOUND::DestroySound	(m_NightVisionOffSnd);
	HUD_SOUND::DestroySound	(m_NightVisionIdleSnd);
	HUD_SOUND::DestroySound	(m_NightVisionBrokenSnd);
}

void CCustomOutfit::net_Export(NET_Packet& P)
{
	inherited::net_Export	(P);
	P.w_float_q8			(m_fCondition,0.0f,1.0f);
	P.w_u8(m_bNightVisionOn ? 1 : 0);
}

void CCustomOutfit::net_Import(NET_Packet& P)
{
	inherited::net_Import	(P);
	P.r_float_q8			(m_fCondition,0.0f,1.0f);
	bool new_bNightVisionOn = !!P.r_u8();

	if (new_bNightVisionOn != m_bNightVisionOn)	
		SwitchNightVision(new_bNightVisionOn);
}

void CCustomOutfit::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_HitTypeProtection[ALife::eHitTypeBurn]		= pSettings->r_float(section,"burn_protection");
	m_HitTypeProtection[ALife::eHitTypeStrike]		= pSettings->r_float(section,"strike_protection");
	m_HitTypeProtection[ALife::eHitTypeShock]		= pSettings->r_float(section,"shock_protection");
	m_HitTypeProtection[ALife::eHitTypeWound]		= pSettings->r_float(section,"wound_protection");
	m_HitTypeProtection[ALife::eHitTypeRadiation]	= pSettings->r_float(section,"radiation_protection");
	m_HitTypeProtection[ALife::eHitTypeTelepatic]	= pSettings->r_float(section,"telepatic_protection");
	m_HitTypeProtection[ALife::eHitTypeChemicalBurn]= pSettings->r_float(section,"chemical_burn_protection");
	m_HitTypeProtection[ALife::eHitTypeExplosion]	= pSettings->r_float(section,"explosion_protection");
	m_HitTypeProtection[ALife::eHitTypeFireWound]	= pSettings->r_float(section,"fire_wound_protection");
	m_HitTypeProtection[ALife::eHitTypePhysicStrike]= READ_IF_EXISTS(pSettings, r_float, section, "physic_strike_protection", 0.0f);

	if (pSettings->line_exist(section, "actor_visual"))
		m_ActorVisual = pSettings->r_string(section, "actor_visual");
	else
		m_ActorVisual = NULL;

	if (pSettings->line_exist(section, "actor_visual_legs"))
		m_ActorVisual_legs = pSettings->r_string(section, "actor_visual_legs");
	else
		m_ActorVisual_legs = NULL;

	m_ef_equipment_type		= pSettings->r_u32(section,"ef_equipment_type");
	if (pSettings->line_exist(section, "power_loss"))
		m_fPowerLoss = pSettings->r_float(section, "power_loss");
	else
		m_fPowerLoss = 1.0f;	

	m_additional_weight				= pSettings->r_float(section,"additional_inventory_weight");
	m_additional_weight2			= pSettings->r_float(section,"additional_inventory_weight2");

	if (pSettings->line_exist(section, "nightvision_sect"))
		m_NightVisionSect = pSettings->r_string(section, "nightvision_sect");
	else
		m_NightVisionSect = NULL;

	m_bNightVisionEnabled	= !!pSettings->r_bool(section,"night_vision");
	if(m_bNightVisionEnabled)
	{
		HUD_SOUND::LoadSound(section,"snd_night_vision_on"	, m_NightVisionOnSnd	, SOUND_TYPE_ITEM_USING);
		HUD_SOUND::LoadSound(section,"snd_night_vision_off"	, m_NightVisionOffSnd	, SOUND_TYPE_ITEM_USING);
		HUD_SOUND::LoadSound(section,"snd_night_vision_idle", m_NightVisionIdleSnd	, SOUND_TYPE_ITEM_USING);
		HUD_SOUND::LoadSound(section,"snd_night_vision_broken", m_NightVisionBrokenSnd, SOUND_TYPE_ITEM_USING);
	}

	m_full_icon_name								= pSettings->r_string(section,"full_icon_name");
}

void CCustomOutfit::SwitchNightVision()
{
	if (OnClient()) return;
	SwitchNightVision(!m_bNightVisionOn);	
}

void CCustomOutfit::SwitchNightVision(bool vision_on)
{
	if(!m_bNightVisionEnabled) return;
	
	m_bNightVisionOn = vision_on;

	CActor *pA = smart_cast<CActor*>(H_Parent());

	if(!pA)					return;
	bool bPlaySoundFirstPerson = (pA == Level().CurrentViewEntity());

	LPCSTR disabled_names	= pSettings->r_string(cNameSect(),"disabled_maps");
	LPCSTR curr_map			= *Level().name();
	u32 cnt					= _GetItemCount(disabled_names);
	bool b_allow			= true;
	string512				tmp;
	for(u32 i=0; i<cnt;++i)
	{
		_GetItem(disabled_names, i, tmp);
		if(0==stricmp(tmp, curr_map))
		{
			b_allow = false;
			break;
		}
	}

	if(m_NightVisionSect.size()&&!b_allow)
	{
		HUD_SOUND::PlaySound(m_NightVisionBrokenSnd, pA->Position(), pA, bPlaySoundFirstPerson);
		return;
	}

	if(m_bNightVisionOn)
	{
		CEffectorPP* pp = pA->Cameras().GetPPEffector((EEffectorPPType)effNightvision);
		if(!pp)
		{
			if (m_NightVisionSect.size())
			{
				AddEffector(pA,effNightvision, m_NightVisionSect);
				HUD_SOUND::PlaySound(m_NightVisionOnSnd, pA->Position(), pA, bPlaySoundFirstPerson);
				HUD_SOUND::PlaySound(m_NightVisionIdleSnd, pA->Position(), pA, bPlaySoundFirstPerson, true);
			}
		}
	} else {
 		CEffectorPP* pp = pA->Cameras().GetPPEffector((EEffectorPPType)effNightvision);
		if(pp)
		{
			pp->Stop			(1.0f);
			HUD_SOUND::PlaySound(m_NightVisionOffSnd, pA->Position(), pA, bPlaySoundFirstPerson);
			HUD_SOUND::StopSound(m_NightVisionIdleSnd);
		}
	}
}

void CCustomOutfit::net_Destroy() 
{
	SwitchNightVision		(false);

	inherited::net_Destroy	();
}

BOOL CCustomOutfit::net_Spawn(CSE_Abstract* DC)
{
	BOOL R		= inherited::net_Spawn	(DC);

	SwitchNightVision		(false);

	return R;
}

void CCustomOutfit::OnH_B_Independent	(bool just_before_destroy) 
{
	inherited::OnH_B_Independent	(just_before_destroy);

	SwitchNightVision			(false);

	HUD_SOUND::StopSound		(m_NightVisionOnSnd);
	HUD_SOUND::StopSound		(m_NightVisionOffSnd);
	HUD_SOUND::StopSound		(m_NightVisionIdleSnd);
}

void CCustomOutfit::Hit(float hit_power, ALife::EHitType hit_type)
{
	hit_power *= m_HitTypeK[hit_type];
	ChangeCondition(-hit_power);
}

float CCustomOutfit::GetDefHitTypeProtection(ALife::EHitType hit_type)
{
	return 1.0f - m_HitTypeProtection[hit_type]*GetCondition();
}

float CCustomOutfit::GetHitTypeProtection(ALife::EHitType hit_type, s16 element)
{
	float fBase = m_HitTypeProtection[hit_type]*GetCondition();
	float bone = m_boneProtection->getBoneProtection(element);
	return 1.0f - fBase*bone;
}

float	CCustomOutfit::HitThruArmour(float hit_power, s16 element, float AP)
{
	float BoneArmour = m_boneProtection->getBoneArmour(element)*GetCondition()*(1-AP);	
	float NewHitPower = hit_power - BoneArmour;
	if (NewHitPower < hit_power*m_boneProtection->m_fHitFrac) return hit_power*m_boneProtection->m_fHitFrac;
	return NewHitPower;
};

BOOL	CCustomOutfit::BonePassBullet					(int boneID)
{
	return m_boneProtection->getBonePassBullet(s16(boneID));
};

void	CCustomOutfit::OnMoveToSlot		()
{
	if (m_pCurrentInventory)
	{
		CActor* pActor = smart_cast<CActor*> (m_pCurrentInventory->GetOwner());
		if (pActor)
		{
			SwitchNightVision(false);

			if (pActor->IsFirstEye() && IsGameTypeSingle() && !pActor->IsActorShadowsOn())
			{
				if (m_ActorVisual_legs.size())
				{
						shared_str NewVisual = m_ActorVisual_legs;
						pActor->ChangeVisual(NewVisual);
				} else {
						shared_str NewVisual = pActor->GetDefaultVisualOutfit_legs();
						pActor->ChangeVisual(NewVisual);
				}
				if(pSettings->line_exist(cNameSect(),"bones_koeff_protection")){
					m_boneProtection->reload( pSettings->r_string(cNameSect(),"bones_koeff_protection"), smart_cast<IKinematics*>(pActor->Visual()) );
				};
			} else {
				if (m_ActorVisual.size())
				{
					shared_str NewVisual = NULL;
					char* TeamSection = Game().getTeamSection(pActor->g_Team());
					if (TeamSection)
					{
						if (pSettings->line_exist(TeamSection, *cNameSect()))
						{
							NewVisual = pSettings->r_string(TeamSection, *cNameSect());
							string256 SkinName;
							xr_strcpy(SkinName, pSettings->r_string("mp_skins_path", "skin_path"));
							xr_strcat(SkinName, *NewVisual);
							xr_strcat(SkinName, ".ogf");
							NewVisual._set(SkinName);
						}
					}
				
					if (!NewVisual.size())
						NewVisual = m_ActorVisual;
	
					pActor->ChangeVisual(NewVisual);
				} else {
					shared_str NewVisual = pActor->GetDefaultVisualOutfit();
					pActor->ChangeVisual(NewVisual);
				}
				if(pSettings->line_exist(cNameSect(),"bones_koeff_protection")){
					m_boneProtection->reload( pSettings->r_string(cNameSect(),"bones_koeff_protection"), smart_cast<IKinematics*>(pActor->Visual()) );
				};
			}
		}
	}
};

void	CCustomOutfit::OnMoveToRuck		()
{
	if (m_pCurrentInventory)
	{
		CActor* pActor = smart_cast<CActor*> (m_pCurrentInventory->GetOwner());
		if (pActor)
		{
			CCustomOutfit* outfit	= pActor->GetOutfit();
			if (!outfit)
			{
				SwitchNightVision(false);

				if (pActor->IsFirstEye() && IsGameTypeSingle())
				{
					shared_str DefVisual = pActor->GetDefaultVisualOutfit_legs();
					if (DefVisual.size())
					{
						pActor->ChangeVisual(DefVisual);
					}
				} else {
					shared_str DefVisual = pActor->GetDefaultVisualOutfit();
					if (DefVisual.size())
					{
						pActor->ChangeVisual(DefVisual);
					}
				}
			}
		}
	}
};

u32	CCustomOutfit::ef_equipment_type	() const
{
	return		(m_ef_equipment_type);
}

float CCustomOutfit::GetPowerLoss() 
{
	if (m_fPowerLoss<1 && GetCondition() <= 0)
	{
		return 1.0f;			
	};
	return m_fPowerLoss;
};
