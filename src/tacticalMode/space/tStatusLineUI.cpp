#include <Urho3D/Scene/Node.h>
#include <Urho3D/UI/Text.h>

#include "tacticalMode/managers/tEquipmentManager.h"
#include "tacticalMode/equipment/weapon/weapon.h"
#include "tacticalMode/spaceship/tEnergyShield.h"
#include "tacticalMode/playerLogic/tTeam.h"
#include "tacticalMode/managers/tResourceManager.h"

#include "tacticalMode/space/tStatusLineUI.h"

TStatusLineUI::TStatusLineUI(Context* context, Node* controlNode, UIElement* uiSpaceState)
	: Object(context)
	, controlNode_(controlNode)
{
	choosenWeaponsText_ = static_cast<Text*>     (uiSpaceState->GetChild("choosenWeaponsText_", true));
	shieldText_ = static_cast<Text*>             (uiSpaceState->GetChild("shieldText_", true));

	resourceValueText_.Resize(TGameResource::ResourceType::Count);
	resourceValueText_[TGameResource::ResourceType::Energy] = static_cast<Text*>    (uiSpaceState->GetChild("energyValue_", true));
	resourceValueText_[TGameResource::ResourceType::Metals] = static_cast<Text*>    (uiSpaceState->GetChild("metalsValue_", true));
}

TStatusLineUI::~TStatusLineUI()
{
}

void TStatusLineUI::UpdateShipInfo() const
{
	if (!controlNode_)
		return;

	// weapon
	String weaponLine = "";

	TEquipmentManager* eManager = controlNode_->GetComponent<TEquipmentManager>();
	if (!eManager)
		return;

	Vector<Node*> weaponNodes = eManager->GetElementNodes(TEquipment::EquipmentType::ET_Weapon);
	for (Node* weaponNode : weaponNodes)
		weaponLine += weaponNode->GetName() + " ";

	// shield
	TEnergyShield* shield = controlNode_->GetComponent<TEnergyShield>();
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

	for (U32 i = 0; i < TGameResource::ResourceType::Count; i++)
	{
		resourceValueText_[i]->SetText(String(static_cast<S32>(pRManager->GetResourceValue(static_cast<TGameResource::ResourceType>(i)))));
		break;
	}
}
