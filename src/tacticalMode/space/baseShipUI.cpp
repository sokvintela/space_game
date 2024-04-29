#include <Urho3D/Scene/Node.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Core/Context.h>

#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/playerLogic/lResearchTree.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "legacyMode/utility/lSharedData.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "prefabs/prefabsStorage.h"
#include "utility/generalMethods.h"
#include "tacticalMode/managers/tEquipmentManager.h"
#include "tacticalMode/equipment/weapon/weapon.h"
#include "equipment/shipEngine.h"
#include "tacticalMode/gameEvents/tGameEvents.h"

#include "tacticalMode/space/baseShipUI.h"

BaseShipUI::BaseShipUI(Context* context, Node* controlNode, UIElement* uiSpaceState)
	: Object(context)
	, controlNode_(controlNode)
{
	equipmentList_.Resize(TEquipment::EquipmentType::ET_Count);
	equipmentList_[0] = static_cast<ListView*>    (uiSpaceState->GetChild("weaponList_", true));
	equipmentList_[2] = static_cast<ListView*>    (uiSpaceState->GetChild("enginesList_", true));

	Network* network = context->GetSubsystem<Network>();
	network->RegisterRemoteEvent(RE_CHANGE_EQUIPMENT);
}

BaseShipUI::~BaseShipUI()
{
}

void BaseShipUI::HandleUpdateUI()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	PrefabsStorage* prefabsStorage = GetSubsystem<PrefabsStorage>();

	for (U32 type = 0; type < TEquipment::EquipmentType::ET_Count; type++)
	{
		TEquipment::EquipmentType eType = static_cast<TEquipment::EquipmentType>(type);
		if (eType == TEquipment::EquipmentType::ET_FieldGenerator)
			continue;

		equipmentList_[type]->SetVisible(true);
		equipmentList_[type]->RemoveAllItems();

		const HashMap<Urho3D::String, Urho3D::Node*> equipment = prefabsStorage->GetAllEquipment(eType);
		for (auto item : equipment)
		{
			UIElement* equipmentBar = new UIElement(context_);
			equipmentBar->SetLayout(LM_HORIZONTAL, 15);
			equipmentBar->SetStyleAuto();

			Button* equipmentButton = equipmentBar->CreateChild<Button>();
			equipmentButton->SetAlignment(HA_LEFT, VA_BOTTOM);
			equipmentButton->SetFixedSize(70, 30);
			equipmentButton->SetTexture(GeneralMethods::GetTextureFromNode(item.second_));
			equipmentButton->SetVar("type", type);

			equipmentMapping_[equipmentButton] = item.second_;

			Text* equipmentName = equipmentBar->CreateChild<Text>();
			equipmentName->SetStyleAuto();
			equipmentName->SetText(item.first_);
			equipmentName->SetAlignment(HA_LEFT, VA_CENTER);

			auto cost = prefabsStorage->GetProbeCreateCost();
			LGeneralMethods::ShowCost(equipmentButton, controlNode_, cost.first_, cost.second_);

			equipmentList_[type]->AddItem(equipmentBar);

			SubscribeToEvent(equipmentButton, E_PRESSED, URHO3D_HANDLER(BaseShipUI, HandleCreateEquipment));
		}
	}
}

void BaseShipUI::SwitchItemUI() const
{
}

void BaseShipUI::SetVisibility(bool visibility)
{
	for (U32 type = 0; type < TEquipment::EquipmentType::ET_Count; type++)
	{
		if (type == TEquipment::EquipmentType::ET_FieldGenerator)
			continue;

		equipmentList_[type]->SetVisible(visibility);
	}
}

void BaseShipUI::HandleCreateEquipment(StringHash eventType, VariantMap & eventData)
{
	Button* selectedButton = static_cast<Button*>(eventData[Pressed::P_ELEMENT].GetPtr());
	for (auto item : equipmentMapping_)
		if (item.first_ == selectedButton)
		{
			if (!controlNode_)
				continue;

			VariantMap changeEquipmentEventData;
			changeEquipmentEventData[ChangeEquipmentEvent::PLAYER_ID] = controlNode_->GetID();
			changeEquipmentEventData[ChangeEquipmentEvent::EQUIPMENT_NAME] = item.second_->GetName();
			changeEquipmentEventData[ChangeEquipmentEvent::EQUIPMENT_TYPE] = selectedButton->GetVar("type");

			GetSubsystem<LSharedData>()->SendGameEvent(RE_CHANGE_EQUIPMENT, changeEquipmentEventData);
		}
}
