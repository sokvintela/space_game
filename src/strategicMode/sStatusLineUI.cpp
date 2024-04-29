#include <Urho3D/Scene/Node.h>
#include <Urho3D/UI/Text.h>

#include "tacticalMode/managers/tEquipmentManager.h"
#include "tacticalMode/equipment/weapon/weapon.h"
#include "legacyMode/spaceship/lEnergyShield.h"
#include "tacticalMode/playerLogic/tTeam.h"
#include "tacticalMode/managers/tResourceManager.h"

#include "strategicMode/sStatusLineUI.h"

SStatusLineUI::SStatusLineUI(Context* context, Node* controlNode, UIElement* uiSpaceState)
	: Object(context)
	, controlNode_(controlNode)
{
	choosenWeaponsText_ = static_cast<Text*>     (uiSpaceState->GetChild("choosenWeaponsText_", true));
	shieldText_ = static_cast<Text*>             (uiSpaceState->GetChild("shieldText_", true));

	resourceValueText_.Resize(TGameResource::ResourceType::Count);
	resourceValueText_[TGameResource::ResourceType::Energy] = static_cast<Text*>    (uiSpaceState->GetChild("energyValue_", true));
	resourceValueText_[TGameResource::ResourceType::Metals] = static_cast<Text*>    (uiSpaceState->GetChild("metalsValue_", true));
	//resourceValueText_[TGameResource::ResourceType::Knowledge] = static_cast<Text*> (uiSpaceState->GetChild("knowledgeValue_", true));
}

SStatusLineUI::~SStatusLineUI()
{
}

void SStatusLineUI::UpdateShipInfo() const
{
	// weapon
	String leftWeaponString = "Left weapon NaN";
	String rightWeaponString = "Right Weapon NaN";

	TEquipmentManager* equipmentManager = controlNode_->GetComponent<TEquipmentManager>();
	if (!equipmentManager)
		return;
	/*
	Weapon* rightWeapon = equipmentManager->GetWeapon(Modules::Module::M_SecondWeaponPlace);
	if (rightWeapon)
		rightWeaponString = rightWeapon->GetNode()->GetName();

	Weapon* leftWeapon = equipmentManager->GetWeapon(Modules::Module::M_FirstWeaponPlace);
	if (leftWeapon)
		leftWeaponString = leftWeapon->GetNode()->GetName();
		*/
	String weaponsString = leftWeaponString + " " + rightWeaponString;
	if (choosenWeaponsText_)
		choosenWeaponsText_->SetText(weaponsString);

	// shield
	LEnergyShield* shield = controlNode_->GetComponent<LEnergyShield>();
	if (shield && shieldText_)
	{
		shieldText_->SetVisible(true);
		shieldText_->SetText("Shield : " + String(static_cast<S32>(shield->GetValue())));
	}
	else
		shieldText_->SetVisible(false);

	// resources
	// player
	TResourceManager* pRManager= controlNode_->GetComponent<TResourceManager>();
	if (!pRManager)
		return;

	// team
	TTeam* team = controlNode_->GetParentComponent<TTeam>(true);
	if (!team)
		return;

	TResourceManager* tRManager = team->GetNode()->GetComponent<TResourceManager>();

	for (U32 i = 0; i < TGameResource::ResourceType::Count; i++)
	{
		TGameResource::ResourceType type = static_cast<TGameResource::ResourceType>(i);
		switch (type)
		{
			case TGameResource::ResourceType::Energy:
			case TGameResource::ResourceType::Metals:
				resourceValueText_[i]->SetText(String(static_cast<S32>(pRManager->GetResourceValue(type))));
				break;
				/*
			case TGameResource::ResourceType::Knowledge:
			{
				if (tRManager)
					resourceValueText_[i]->SetText(String(static_cast<S32>(tRManager->GetResourceValue(type))));
				else
					resourceValueText_[i]->SetText("0");

				break;
			}
			*/
		}
	}
}
