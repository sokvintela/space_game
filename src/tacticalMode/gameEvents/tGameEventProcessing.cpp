#include <Urho3D/Network/Network.h>

#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/gameEvents/tGameEvents.h"
#include "prefabs/prefabsStorage.h"
#include "utility/generalMethods.h"
#include "tacticalMode/managers/tEquipmentManager.h"
#include "tacticalMode/equipment/weapon/weapon.h"

#include "tacticalMode/gameEvents/tGameEventProcessing.h"

using namespace Urho3D;

TGameEventProcessing::TGameEventProcessing(Urho3D::Context* context) :
	Object(context)
{
}

TGameEventProcessing::~TGameEventProcessing()
{
}

void TGameEventProcessing::SubscribeToNetworkEvents()
{
	SubscribeToEvent(RE_EQUIPMENT_MOVE, URHO3D_HANDLER(TGameEventProcessing, HandleEquipmentMove));
	SubscribeToEvent(RE_CHANGE_EQUIPMENT, URHO3D_HANDLER(TGameEventProcessing, HandleChangeEquipment));
}

void TGameEventProcessing::SetScene(Scene * scene)
{
	scene_ = scene;
}

bool TGameEventProcessing::IsServerConnection() const
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return true;

	return false;
}

void TGameEventProcessing::HandleEquipmentMove(StringHash eventType, VariantMap & eventData)
{
	U32 nodeID = eventData[EquipmentMoveEvent::NODE_ID].GetUInt();
	U32 newParentID = eventData[EquipmentMoveEvent::NEW_PARENT_ID].GetUInt();

	Node* node = scene_->GetNode(nodeID);
	Node* newParent = scene_->GetNode(newParentID);

	if (node && newParent)
		newParent->AddChild(node);
}

void TGameEventProcessing::HandleChangeEquipment(StringHash eventType, VariantMap & eventData)
{
	S32 playerID = eventData[ChangeEquipmentEvent::PLAYER_ID].GetUInt();
	String equipmentName = eventData[ChangeEquipmentEvent::EQUIPMENT_NAME].GetString();
	U32 typeNumber = eventData[ChangeEquipmentEvent::EQUIPMENT_TYPE].GetInt();

	TEquipment::EquipmentType type = static_cast<TEquipment::EquipmentType>(typeNumber);
	Node* controlNode = GeneralMethods::GetObjectNode(scene_, playerID);
	if (!controlNode)
		return;

	TEquipmentManager* eManager = controlNode->GetComponent<TEquipmentManager>();
	if (!eManager)
		return;

	Node* equipmentNode = nullptr;
	if (type == TEquipment::EquipmentType::ET_Weapon)
	{
		F32 minDamage = INFINITY;
		Vector<Node*> weaponNodes = eManager->GetElementNodes(type);
		for (Node* weaponNode : weaponNodes)
		{
			Weapon* weapon = weaponNode->GetComponent<Weapon>();
			if (weapon->GetDamage() < minDamage)
			{
				minDamage = weapon->GetDamage();
				equipmentNode = weaponNode;
				break;
			}
		}
	}

	TEquipmentManager::Module module = eManager->GetModuleByEquipment(type, equipmentNode);
	Node* elementNode = GetSubsystem<PrefabsStorage>()->CreateEquipment(controlNode, type, equipmentName);
	if (elementNode)
		eManager->AddElementToModule(module, elementNode);
}
