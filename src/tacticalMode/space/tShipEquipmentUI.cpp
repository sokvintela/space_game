#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>

#include "utility/sharedData.h"
#include "equipment/shipEngine.h"
#include "tacticalMode/managers/tEquipmentManager.h"
#include "tacticalMode/gameEvents/tGameEvents.h"
#include "utility/generalMethods.h"
#include "tacticalMode/equipment/weapon/weapon.h"

#include "tacticalMode/space/tShipEquipmentUI.h"

using namespace Urho3D;

TShipEquipment::TShipEquipment(Context * context, Node* controlNode, UIElement* uiSpaceState)
	: Object(context)
	, controlNode_(controlNode)
	, dragBeginPosition_(IntVector2::ZERO)
{
	// ship info
	equipmentBar_ = uiSpaceState->GetChild("equipmentBar_", true);
	modulesBar_ = uiSpaceState->GetChild("modulesBar_", true);
	cargoBar_ = uiSpaceState->GetChild("cargoBar_", true);

	CreateShipInfo();
}

TShipEquipment::~TShipEquipment()
{
	UnsubscribeFromEvent(E_DRAGBEGIN);
	UnsubscribeFromEvent(E_DRAGMOVE);
	UnsubscribeFromEvent(E_DRAGEND);
	UnsubscribeFromEvent(E_NODEADDED);
}

void TShipEquipment::CreateShipInfo()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/parts/modules.xml");
	modulesBar_->LoadXML(layout->GetRoot(), style);

	leftWeaponPlace_ = static_cast<Sprite*>(modulesBar_->GetChild("leftWing_", true));
	rightWeaponPlace_ = static_cast<Sprite*>(modulesBar_->GetChild("rightWing_", true));
	enginePlace_ = static_cast<Sprite*>(modulesBar_->GetChild("base_", true));

	leftWeaponPlace_->SetEnabled(true);
	leftWeaponPlace_->SetVar("moduleType", TEquipmentManager::Module::M_FirstWeapon);
	rightWeaponPlace_->SetEnabled(true);
	rightWeaponPlace_->SetVar("moduleType", TEquipmentManager::Module::M_SecondWeapon);
	enginePlace_->SetEnabled(true);
	enginePlace_->SetVar("moduleType", TEquipmentManager::Module::M_Engine);

	HandleShipInfoUpdate();

	SubscribeToEvent(E_DRAGBEGIN, URHO3D_HANDLER(TShipEquipment, HandleDragBegin));
	SubscribeToEvent(E_DRAGMOVE, URHO3D_HANDLER(TShipEquipment, HandleDragMove));
	SubscribeToEvent(E_DRAGEND, URHO3D_HANDLER(TShipEquipment, HandleDragEnd));

	SubscribeToEvent(E_NODEADDED, URHO3D_HANDLER(TShipEquipment, HandleNodeAdded));
}

void TShipEquipment::HandleShipInfoUpdate()
{
	TEquipmentManager* eManager = controlNode_->GetComponent<TEquipmentManager>();
	if (!eManager)
		return;

	if (leftWeaponButton_)
		leftWeaponButton_->Remove();
	if (rightWeaponButton_)
		rightWeaponButton_->Remove();
	if (engineButton_)
		engineButton_->Remove();

	cargoBar_->RemoveAllChildren();
	equipmentBar_->RemoveAllChildren();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	Texture2D* cellTexture = cache->GetResource<Texture2D>("Textures/cell.png");

	static U32 equipmentMaxSize = 6;

	// equipment
	UIElement* equipmentElement = nullptr;
	auto equipmentChildren = eManager->GetElementNodes(TEquipmentManager::Module::M_Cargo);
	for (U32 i = 0; i < equipmentMaxSize; i++)
	{
		if (i % 3 == 0)
		{
			equipmentElement = equipmentBar_->CreateChild<UIElement>(String::EMPTY, i / 3);
			equipmentElement->SetLayout(LM_VERTICAL);
			equipmentElement->SetAlignment(HA_LEFT, VA_TOP);
		}

		UIElement* equipmentCell = equipmentElement->CreateChild<UIElement>(String::EMPTY, i % 3);
		equipmentCell->SetLayout(LM_FREE);

		Sprite* sprite = equipmentCell->CreateChild<Sprite>(String::EMPTY, 0);
		sprite->SetFixedSize(32, 32);
		sprite->SetTexture(cellTexture);
		sprite->SetDragDropMode(DD_TARGET);
		sprite->SetEnabled(true);
		sprite->SetVar("moduleType", TEquipmentManager::Module::M_Cargo);

		if (i >= equipmentChildren.Size())
			continue;

		Button* equipmentButton = equipmentCell->CreateChild<Button>(String::EMPTY, 1);
		equipmentButton->SetFixedSize(32, 32);

		Texture2D* equipmentTexture = GeneralMethods::GetTextureFromNode(equipmentChildren[i]);
		if (equipmentTexture)
		{
			equipmentButton->SetTexture(equipmentTexture);
			equipmentButton->SetDragDropMode(DD_SOURCE);
			equipmentButton->SetVar("NodePtr", equipmentChildren[i].Get());
		}
	}

/*
	for (unsigned i = 0; i < cargoBar_->GetNumChildren(); i++)
	{
		UIElement* cargoElement = cargoBar_->GetChild(i);
		if (cargoElement->GetNumChildren() > 1)
			cargoElement->RemoveChildAtIndex(1);
	}
*/

	// cargo
	UIElement* cargoElement;
	auto cargoChildren = eManager->GetElementNodes(TEquipmentManager::Module::M_Cargo);
	for (U32 i = 0; i <= cargoChildren.Size(); i++)
	{
		/* TODO
		UIElement* elementBar = new UIElement(context_);
		elementBar->SetLayout(LM_HORIZONTAL, 15);
		elementBar->SetStyleAuto();
		*/

		if (i % 3 == 0)
		{
			cargoElement = cargoBar_->CreateChild<UIElement>(String::EMPTY, i / 3);
			cargoElement->SetLayout(LM_VERTICAL);
			cargoElement->SetAlignment(HA_LEFT, VA_TOP);
		}

		UIElement* cargoCell = cargoElement->CreateChild<UIElement>(String::EMPTY, i % 3);
		cargoCell->SetLayout(LM_FREE);
		cargoCell->SetFixedSize(32, 32);

		Sprite* sprite = cargoCell->CreateChild<Sprite>(String::EMPTY, i % 3);
		sprite->SetFixedSize(32, 32);
		sprite->SetTexture(cellTexture);
		sprite->SetDragDropMode(DD_TARGET);
		sprite->SetEnabled(true);
		sprite->SetVar("moduleType", TEquipmentManager::Module::M_Count);

		/// empty cell for the last cargo cell
		if (i >= cargoChildren.Size())
			break;

		Button* cargoButton = cargoCell->CreateChild<Button>(String::EMPTY, 1);
		cargoButton->SetFixedSize(32, 32);

		Texture2D* cargoTexture = GeneralMethods::GetTextureFromNode(equipmentChildren[i]);
		if (cargoTexture)
		{
			cargoButton->SetTexture(cargoTexture);
			cargoButton->SetDragDropMode(DD_SOURCE);
			cargoButton->SetVar("NodePtr", cargoChildren[i].Get());
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
	Node* leftWeaponNode = eManager->GetElementNode<Weapon>(TEquipmentManager::Module::M_FirstWeapon);
	if (leftWeaponNode)
	{
		Texture2D* texture = GeneralMethods::GetTextureFromNode(leftWeaponNode);

		leftWeaponButton_ = leftWeaponPlace_->CreateChild<Button>();
		leftWeaponButton_->SetFixedSize(30, 30);
		leftWeaponButton_->SetTexture(texture);
		leftWeaponButton_->SetDragDropMode(DD_SOURCE);
		leftWeaponButton_->SetAlignment(HA_CENTER, VA_CENTER);
		leftWeaponButton_->SetVar("NodePtr", leftWeaponNode);
	}

	Node* rightWeaponNode = eManager->GetElementNode<Weapon>(TEquipmentManager::Module::M_SecondWeapon);
	if (rightWeaponNode)
	{
		Texture2D* texture = GeneralMethods::GetTextureFromNode(rightWeaponNode);

		rightWeaponButton_ = rightWeaponPlace_->CreateChild<Button>();
		rightWeaponButton_->SetFixedSize(30, 30);
		rightWeaponButton_->SetTexture(texture);
		rightWeaponButton_->SetDragDropMode(DD_SOURCE);
		rightWeaponButton_->SetAlignment(HA_CENTER, VA_CENTER);
		rightWeaponButton_->SetVar("NodePtr", rightWeaponNode);
	}

	Node* shipEngineNode = eManager->GetElementNode<ShipEngine>(TEquipmentManager::Module::M_Engine);
	if (shipEngineNode)
	{
		Texture2D* texture = GeneralMethods::GetTextureFromNode(shipEngineNode);

		engineButton_ = enginePlace_->CreateChild<Button>();
		engineButton_->SetFixedSize(30, 30);
		engineButton_->SetTexture(texture);
		engineButton_->SetDragDropMode(DD_SOURCE);
		engineButton_->SetAlignment(HA_CENTER, VA_CENTER);
		engineButton_->SetVar("NodePtr", shipEngineNode);
	}
}

void TShipEquipment::HandleDragBegin(StringHash eventType, VariantMap& eventData)
{
	Button* draggedButton = dynamic_cast<Button*>(eventData[DragBegin::P_ELEMENT].GetPtr());
	if (!draggedButton)
		return;

	dragBeginElementPosition_ = draggedButton->GetPosition();
	dragBeginPosition_ = IntVector2(eventData[DragBegin::P_X].GetInt(), eventData[DragBegin::P_Y].GetInt());

	Node* draggedEquipmentNode = static_cast<Node*>(draggedButton->GetVar("NodePtr").GetPtr());
	if (!draggedEquipmentNode)
		return;

	Weapon* isWeapon = draggedEquipmentNode->GetDerivedComponent<Weapon>();
	ShipEngine* isEngine = draggedEquipmentNode->GetComponent<ShipEngine>();
	bool isEquipment = (isEngine == nullptr && isWeapon == nullptr);

	if (isWeapon)
	{
		leftWeaponPlace_->SetColor(Color::CYAN);
		rightWeaponPlace_->SetColor(Color::CYAN);
	}

	if (isEngine)
	{
		enginePlace_->SetColor(Color::CYAN);
	}

	if (isEquipment)
	{
		for(auto eCell : equipmentBar_->GetChildren())
		{
			eCell->SetColor(Color::CYAN);
		}
	}
}

void TShipEquipment::HandleDragMove(StringHash eventType, VariantMap& eventData)
{
	Button* draggedButton = dynamic_cast<Button*>(eventData[DragMove::P_ELEMENT].GetPtr());
	if (!draggedButton)
		return;

	IntVector2 dragCurrentPosition = IntVector2(eventData[DragMove::P_X].GetInt(), eventData[DragMove::P_Y].GetInt());
	draggedButton->SetPosition(dragCurrentPosition - dragBeginPosition_);
}

void TShipEquipment::HandleDragEnd(StringHash eventType, VariantMap& eventData)
{
	TEquipmentManager* eManager = controlNode_->GetComponent<TEquipmentManager>();
	if (!eManager)
		return;

	Button* draggedButton = dynamic_cast<Button*>(eventData[DragEnd::P_ELEMENT].GetPtr());
	if (!draggedButton)
		return;

	IntVector2 dragCurrentPosition = IntVector2(eventData[DragMove::P_X].GetInt(), eventData[DragMove::P_Y].GetInt());

	// TODO: remove workaround with temporary disabling button
	draggedButton->SetEnabled(false);
	UIElement* dragTarget = GetSubsystem<UI>()->GetElementAt(dragCurrentPosition);
	Sprite* targetSlot = dynamic_cast<Sprite*>(dragTarget);
	draggedButton->SetEnabled(true);

	Node* draggedEquipmentNode = static_cast<Node*>(draggedButton->GetVar("NodePtr").GetPtr());
	if (!draggedEquipmentNode)
		return;

	bool correctPlacement = false;
	if (targetSlot)
	{
		TEquipmentManager::Module moduleType = static_cast<TEquipmentManager::Module>(targetSlot->GetVar("moduleType").GetUInt());
		Node* parentModuleNode = (moduleType == TEquipmentManager::Module::M_Count ? eManager->GetOrCreateModuleNode(TEquipmentManager::Module::M_Cargo) : eManager->GetOrCreateModuleNode(moduleType));

		if (IsPlacementCorrect(moduleType, draggedEquipmentNode))
		{
			VariantMap equipmentMoveData;
			equipmentMoveData[EquipmentMoveEvent::NODE_ID] = draggedEquipmentNode->GetID();
			equipmentMoveData[EquipmentMoveEvent::NEW_PARENT_ID] = parentModuleNode->GetID();

			GetSubsystem<SharedData>()->SendGameEvent(RE_EQUIPMENT_MOVE, equipmentMoveData);
			correctPlacement = true;
		}
	}

	if (!correctPlacement)
	{
		draggedButton->SetPosition(dragBeginElementPosition_);
	}

	leftWeaponPlace_->SetColor(Color::WHITE);
	rightWeaponPlace_->SetColor(Color::WHITE);
	enginePlace_->SetColor(Color::WHITE);

	for (auto eCell : equipmentBar_->GetChildren())
	{
		eCell->SetColor(Color::WHITE);
	}
}

void TShipEquipment::HandleNodeAdded(StringHash eventType, VariantMap & eventData)
{
	HandleShipInfoUpdate();
}

bool TShipEquipment::IsPlacementCorrect(TEquipmentManager::Module module, Node* node) const
{
	Weapon* isWeapon = node->GetDerivedComponent<Weapon>();
	ShipEngine* isEngine = node->GetComponent<ShipEngine>();
	bool isEquipment = (isEngine == nullptr && isWeapon == nullptr);

	switch (module)
	{
		case TEquipmentManager::Module::M_FirstWeapon:
		case TEquipmentManager::Module::M_SecondWeapon:
		{
			return (isWeapon != nullptr);
		}
		case TEquipmentManager::Module::M_Engine:
		{
			return (isEngine != nullptr);
		}
		case TEquipmentManager::Module::M_Cargo:
		{
			return isEquipment;
		}
		case TEquipmentManager::Module::M_Count:
		{
			return true;
		}
	}

	return false;
}
