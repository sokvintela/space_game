#include <Urho3D/UI/Button.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include "tacticalMode/equipment/weapon/weapon.h"
#include "equipment/shipEngine.h"
#include "legacyMode/equipment/hyperDrive.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/utility/lSharedData.h"
#include "tacticalMode/gameEvents/tGameEvents.h"

#include "legacyMode/space/SubStates/lSpaceShipInfoState.h"

using namespace Urho3D;

void LGameShipInfoState::RegisterEvents(Context * context)
{
	Network* network = context->GetSubsystem<Network>();
	network->RegisterRemoteEvent(RE_EQUIPMENT_MOVE);
}

LGameShipInfoState::LGameShipInfoState(Urho3D::Context * context, UIElement* uiSubState) :
	ISubState(context, uiSubState)
{
}

void LGameShipInfoState::Enter(Node* playerNode)
{
	playerID_ = playerNode->GetID();
	equipmentManager_ = playerNode->GetComponent<LEquipmentManager>();
	if (!equipmentManager_)
		return;

	leftWing_ = equipmentManager_->GetOrCreateModuleNode(Modules::Module::M_FirstWeaponPlace);
	rightWing_ = equipmentManager_->GetOrCreateModuleNode(Modules::Module::M_SecondWeaponPlace);

	HandleShipInfoUpdate();

	uiSubStateRoot_->SetVisible(true);
	uiSubStateRoot_->UpdateLayout();

	dragBeginPosition_ = IntVector2::ZERO;

	SubscribeToEvent(E_DRAGBEGIN, URHO3D_HANDLER(LGameShipInfoState, HandleDragBegin));
	SubscribeToEvent(E_DRAGMOVE, URHO3D_HANDLER(LGameShipInfoState, HandleDragMove));
	SubscribeToEvent(E_DRAGEND, URHO3D_HANDLER(LGameShipInfoState, HandleDragEnd));

	SubscribeToEvent(E_NODEADDED, URHO3D_HANDLER(LGameShipInfoState, HandleNodeAdded));

	SubscribeToEvent(backToSpace_, E_PRESSED, URHO3D_HANDLER(LGameShipInfoState, HandleBackToGameClick));
}

void LGameShipInfoState::Create(Node* targetNode)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/menuShipInfo.xml");
	uiSubStateRoot_->LoadXML(layout->GetRoot(), style);

	cargoBar_ =             static_cast<UIElement*>(uiSubStateRoot_->GetChild("cargoBar_", true));
	//Sprite* shipSprite =           static_cast<Sprite*>(uiSubStateRoot_->GetChild("shipSprite_", true));
	backToSpace_ =          static_cast<Button*>(uiSubStateRoot_->GetChild("backToSpace_", true));

	leftWeaponPlace_ =      static_cast<Sprite*>(uiSubStateRoot_->GetChild("leftWeaponPlace_", true));
	rightWeaponPlace_ =     static_cast<Sprite*>(uiSubStateRoot_->GetChild("rightWeaponPlace_", true));
	enginePlace_ =          static_cast<Sprite*>(uiSubStateRoot_->GetChild("enginePlace_", true));
	hyperEnginePlace_ =      static_cast<Sprite*>(uiSubStateRoot_->GetChild("hyperEnginePlace_", true));

	leftWeaponPlace_->SetEnabled(true);
	rightWeaponPlace_->SetEnabled(true);
	enginePlace_->SetEnabled(true);
	hyperEnginePlace_->SetEnabled(true);

	slotTypeMapping_[leftWeaponPlace_] = ST_LEFT_WEAPON;
	slotTypeMapping_[rightWeaponPlace_] = ST_RIGHT_WEAPON;
	slotTypeMapping_[enginePlace_] = ST_ENGINE;
	slotTypeMapping_[hyperEnginePlace_] = ST_HYPER_DRIVE;

	Texture2D* cellCargoTexture = cache->GetResource<Texture2D>("Textures/cell.png");

	for (U32 i = 0; i < 5; i++)
	{
		UIElement* cargoElement = cargoBar_->CreateChild<UIElement>(String::EMPTY, i);
		cargoElement->SetLayout(LM_FREE, 10, IntRect(10, 10, 10, 10));
		cargoElement->SetAlignment(HA_LEFT, VA_TOP);
		cargoElement->SetFixedSize(70, 70);

		Sprite* cargoCellSprite = cargoElement->CreateChild<Sprite>(String::EMPTY, 0);
		cargoCellSprite->SetFixedSize(70, 70);
		cargoCellSprite->SetTexture(cellCargoTexture);
		cargoCellSprite->SetDragDropMode(DD_TARGET);
		cargoCellSprite->SetEnabled(true);

		slotTypeMapping_[cargoCellSprite] = ST_CARGO;
	}
}

void LGameShipInfoState::HandleDragBegin(StringHash eventType, VariantMap& eventData)
{
	Button* draggedButton = dynamic_cast<Button*>(eventData[DragBegin::P_ELEMENT].GetPtr());
	if (!draggedButton)
		return;

	dragBeginElementPosition_ = draggedButton->GetPosition();
	dragBeginPosition_ = IntVector2(eventData[DragBegin::P_X].GetInt(), eventData[DragBegin::P_Y].GetInt());

	if (!equipmentMapping_[draggedButton])
		return;

	Weapon* isWeapon = equipmentMapping_[draggedButton]->GetDerivedComponent<Weapon>();
	ShipEngine* isEngine = equipmentMapping_[draggedButton]->GetComponent<ShipEngine>();
	HyperDrive* isHyperEngine = equipmentMapping_[draggedButton]->GetComponent<HyperDrive>();

	if (isWeapon)
	{
		leftWeaponPlace_->SetColor(Color::BLUE);
		rightWeaponPlace_->SetColor(Color::BLUE);
	}

	if (isEngine)
	{
		enginePlace_->SetColor(Color::BLUE);
	}

	if (isHyperEngine)
	{
		hyperEnginePlace_->SetColor(Color::BLUE);
	}
}

void LGameShipInfoState::HandleDragMove(StringHash eventType, VariantMap& eventData)
{
	Button* draggedButton = dynamic_cast<Button*>(eventData[DragMove::P_ELEMENT].GetPtr());
	if (!draggedButton)
		return;

	IntVector2 dragCurrentPosition = IntVector2(eventData[DragMove::P_X].GetInt(), eventData[DragMove::P_Y].GetInt());
	draggedButton->SetPosition(dragCurrentPosition - dragBeginPosition_);
}

void LGameShipInfoState::HandleDragEnd(StringHash eventType, VariantMap& eventData)
{
	Button* draggedButton = dynamic_cast<Button*>(eventData[DragEnd::P_ELEMENT].GetPtr());
	if (!draggedButton)
		return;

	IntVector2 dragCurrentPosition = IntVector2(eventData[DragMove::P_X].GetInt(), eventData[DragMove::P_Y].GetInt());

	UIElement* dragTarget = GetSubsystem<UI>()->GetElementAt(dragCurrentPosition);
	Sprite* targetSlot = dynamic_cast<Sprite*>(dragTarget);

	bool correctPlacement = false;
	if (targetSlot)
	{
		Weapon* isWeapon = equipmentMapping_[draggedButton]->GetDerivedComponent<Weapon>();
		ShipEngine* isEngine = equipmentMapping_[draggedButton]->GetComponent<ShipEngine>();
		HyperDrive* isHyperEngine = equipmentMapping_[draggedButton]->GetComponent<HyperDrive>();

		Node* nodeToMove = equipmentMapping_[draggedButton];
		Node* newParentNode = nullptr;
		switch (slotTypeMapping_[targetSlot])
		{
			case ST_LEFT_WEAPON:
				if (isWeapon)
				{
					newParentNode = leftWing_;
					correctPlacement = true;
				}
				break;
			case ST_RIGHT_WEAPON:
				if (isWeapon)
				{
					newParentNode = rightWing_;
					correctPlacement = true;
				}
				break;
			case ST_ENGINE:
				if (isEngine)
				{
					newParentNode = equipmentManager_->GetModulesNode();
					correctPlacement = true;
				}
				break;
			case ST_HYPER_DRIVE:
				if (isHyperEngine)
				{
					newParentNode = equipmentManager_->GetModulesNode();
					correctPlacement = true;
				}
				break;
			case ST_CARGO:
				newParentNode = equipmentManager_->GetOrCreateCargoNode();
				correctPlacement = true;
				break;
		}

		if (correctPlacement)
		{
			VariantMap equipmentMoveData;
			equipmentMoveData[EquipmentMoveEvent::NODE_ID] = nodeToMove->GetID();
			equipmentMoveData[EquipmentMoveEvent::NEW_PARENT_ID] = newParentNode->GetID();

			GetSubsystem<LSharedData>()->SendGameEvent(RE_EQUIPMENT_MOVE, equipmentMoveData);
		}
	}

	if (!correctPlacement)
	{
		draggedButton->SetPosition(dragBeginElementPosition_);
	}

	leftWeaponPlace_->SetColor(Color::WHITE);
	rightWeaponPlace_->SetColor(Color::WHITE);
	enginePlace_->SetColor(Color::WHITE);
	hyperEnginePlace_->SetColor(Color::WHITE);
}

void LGameShipInfoState::HandleBackToGameClick(StringHash eventType, VariantMap & eventData)
{
	Exit();
	parentState_->Resume();
}

void LGameShipInfoState::HandleNodeAdded(StringHash eventType, VariantMap & eventData)
{
	HandleShipInfoUpdate();
}

void LGameShipInfoState::HandleShipInfoUpdate()
{
	if (leftWeaponButton_)
		leftWeaponButton_->Remove();
	if (rightWeaponButton_)
		rightWeaponButton_->Remove();
	if (engineButton_)
		engineButton_->Remove();
	if (hyperEngineButton_)
		hyperEngineButton_->Remove();

	equipmentMapping_.Clear();
	for (U32 i = 0; i < cargoBar_->GetNumChildren(); i++)
	{
		UIElement* cargoElement = cargoBar_->GetChild(i);
		if (cargoElement->GetNumChildren() > 1)
			cargoElement->RemoveChildAtIndex(1);
	}

	// cargo
	U32 cargoSize = 0;

	auto children = equipmentManager_->GetCargoElements();
	cargoSize = children.Size();
	for (U32 i = 0; i < cargoSize; i++)
	{
		/* TODO
		UIElement* elementBar = new UIElement(context_);
		elementBar->SetLayout(LM_HORIZONTAL, 15);
		elementBar->SetStyleAuto();
		*/

		UIElement* cargoElement = cargoBar_->GetChild(i);

		Button* cargoButton = cargoElement->CreateChild<Button>(String::EMPTY, 1);
		cargoButton->SetFixedSize(70, 70);

		StaticSprite2D* spriteCargo = children[i]->GetComponent<StaticSprite2D>();
		if (spriteCargo)
		{
			Texture2D* cargoTexture = spriteCargo->GetSprite()->GetTexture();
			if (cargoTexture)
			{
				cargoButton->SetTexture(cargoTexture);
				cargoButton->SetDragDropMode(DD_SOURCE);

				equipmentMapping_[cargoButton] = children[i];
			}
		}

		/*
		elementBar->AddChild(cargoElement);

		Text* elementName = elementBar->CreateChild<Text>();
		elementName->SetStyleAuto();

		Weapon* weapon = children[i]->GetDerivedComponent<Weapon>(true);
		if (weapon)
			elementName->SetText(String(weaponTypeNames[weapon->GetUseTypeNumber()]));

		if (children[i]->GetComponent<RepairDrone>(true))
			elementName->SetText("Repair drone");

		if (children[i]->GetComponent<CrystalDetector>(true))
			elementName->SetText("Crystal detector");

		if (children[i]->GetComponent<HyperDrive>(true))
			elementName->SetText("Hyper engine");

		ShipEngine* engine = children[i]->GetComponent<ShipEngine>(true);
		if (engine)
			elementName->SetText(String(engineTypeNames[engine->GetUseTypeNumber()]) + " engine");

		elementBar->AddChild(elementName);
		*/
	}

	// ship weapon imagies

	Weapon* leftWeapon = equipmentManager_->GetWeapon(Modules::Module::M_FirstWeaponPlace);
	if (leftWeapon)
	{
		StaticSprite2D* leftWeaponSprite = leftWeapon->GetComponent<StaticSprite2D>();
		leftWeaponButton_ = leftWeaponPlace_->GetParent()->CreateChild<Button>();
		leftWeaponButton_->SetFixedSize(70, 70);
		leftWeaponButton_->SetTexture(leftWeaponSprite->GetSprite()->GetTexture());
		leftWeaponButton_->SetDragDropMode(DD_SOURCE);

		equipmentMapping_[leftWeaponButton_] = leftWeapon->GetNode();
	}

	Weapon* rightWeapon = equipmentManager_->GetWeapon(Modules::Module::M_SecondWeaponPlace);
	if (rightWeapon)
	{
		StaticSprite2D* rightWeaponSprite = rightWeapon->GetComponent<StaticSprite2D>();
		rightWeaponButton_ = rightWeaponPlace_->GetParent()->CreateChild<Button>();
		rightWeaponButton_->SetFixedSize(70, 70);
		rightWeaponButton_->SetTexture(rightWeaponSprite->GetSprite()->GetTexture());
		rightWeaponButton_->SetDragDropMode(DD_SOURCE);

		equipmentMapping_[rightWeaponButton_] = rightWeapon->GetNode();
	}

	ShipEngine* shipEngine = equipmentManager_->GetEquipmentItem<ShipEngine>();
	if (shipEngine)
	{
		StaticSprite2D* engineSprite = shipEngine->GetComponent<StaticSprite2D>();
		engineButton_ = enginePlace_->GetParent()->CreateChild<Button>();
		engineButton_->SetFixedSize(70, 70);
		engineButton_->SetTexture(engineSprite->GetSprite()->GetTexture());
		engineButton_->SetDragDropMode(DD_SOURCE);

		equipmentMapping_[engineButton_] = shipEngine->GetNode();
	}

	HyperDrive* hyperEngine = equipmentManager_->GetEquipmentItem<HyperDrive>();
	if (hyperEngine)
	{
		StaticSprite2D* hyperEngineSprite = hyperEngine->GetComponent<StaticSprite2D>();
		hyperEngineButton_ = hyperEnginePlace_->GetParent()->CreateChild<Button>();
		hyperEngineButton_->SetFixedSize(70, 70);
		hyperEngineButton_->SetTexture(hyperEngineSprite->GetSprite()->GetTexture());
		hyperEngineButton_->SetDragDropMode(DD_SOURCE);

		equipmentMapping_[hyperEngineButton_] = hyperEngine->GetNode();
	}
}

void LGameShipInfoState::Exit()
{
	uiSubStateRoot_->SetVisible(false);
	UnsubscribeFromAllEvents();
}
