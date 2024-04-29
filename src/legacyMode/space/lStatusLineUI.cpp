#include <Urho3D/Scene/Node.h>
#include <Urho3D/UI/Text.h>

#include "legacyMode/managers/lEquipmentManager.h"
#include "tacticalMode/equipment/weapon/weapon.h"
#include "legacyMode/spaceship/lEnergyShield.h"
#include "legacyMode/playerLogic/lTeam.h"
#include "legacyMode/managers/lResourceManager.h"

#include "legacyMode/space/lStatusLineUI.h"

LStatusLineUI::LStatusLineUI(Context* context, Node* controlNode, UIElement* uiSpaceState)
	: Object(context)
	, controlNode_(controlNode)
{
	choosenWeaponsText_ = static_cast<Text*>     (uiSpaceState->GetChild("choosenWeaponsText_", true));
	shieldText_ = static_cast<Text*>             (uiSpaceState->GetChild("shieldText_", true));

	resourceValueText_.Resize(LGameResource::ResourceType::Count);
	resourceValueText_[LGameResource::ResourceType::Energy] = static_cast<Text*>    (uiSpaceState->GetChild("energyValue_", true));
	resourceValueText_[LGameResource::ResourceType::Metals] = static_cast<Text*>    (uiSpaceState->GetChild("metalsValue_", true));
	resourceValueText_[LGameResource::ResourceType::Knowledge] = static_cast<Text*> (uiSpaceState->GetChild("knowledgeValue_", true));
}

LStatusLineUI::~LStatusLineUI()
{
}

void LStatusLineUI::UpdateShipInfo() const
{
	// weapon
	String leftWeaponString = "Left weapon NaN";
	String rightWeaponString = "Right Weapon NaN";

	LEquipmentManager* equipmentManager = controlNode_->GetComponent<LEquipmentManager>();
	if (!equipmentManager)
		return;

	Weapon* rightWeapon = equipmentManager->GetWeapon(Modules::Module::M_SecondWeaponPlace);
	if (rightWeapon)
		rightWeaponString = rightWeapon->GetNode()->GetName();

	Weapon* leftWeapon = equipmentManager->GetWeapon(Modules::Module::M_FirstWeaponPlace);
	if (leftWeapon)
		leftWeaponString = leftWeapon->GetNode()->GetName();

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
	LResourceManager* pRManager= controlNode_->GetComponent<LResourceManager>();
	if (!pRManager)
		return;

	// team
	LTeam* team = controlNode_->GetParentComponent<LTeam>(true);
	if (!team)
		return;

	LResourceManager* tRManager = team->GetNode()->GetComponent<LResourceManager>();

	for (U32 i = 0; i < LGameResource::ResourceType::Count; i++)
	{
		LGameResource::ResourceType type = static_cast<LGameResource::ResourceType>(i);
		switch (type)
		{
			case LGameResource::ResourceType::Energy:
			case LGameResource::ResourceType::Metals:
				resourceValueText_[i]->SetText(String(static_cast<S32>(pRManager->GetResourceValue(type))));
				break;
			case LGameResource::ResourceType::Knowledge:
			{
				if (tRManager)
					resourceValueText_[i]->SetText(String(static_cast<S32>(tRManager->GetResourceValue(type))));
				else
					resourceValueText_[i]->SetText("0");

				break;
			}
		}
	}
}
