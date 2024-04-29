#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/Network.h>

#include "tacticalMode/ai/mob.h"
#include "legacyMode/managers/lResourceManager.h"
#include "prefabs/prefabsStorage.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/playerLogic/owner.h"
#include "legacyMode/managers/lDataManager.h"

#include "legacyMode/buildings/shipFactory.h"

F32 ShipFactory::creationTime_ = 5000;
F32 ShipFactory::createMobCost_ = 5.f;

void ShipFactory::RegisterObject(Context * context)
{
	context->RegisterFactory<ShipFactory>();

	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("processShips", GetProcessShips, SetProcessShips, PODVector<U8>, Variant::emptyBuffer, AM_DEFAULT);
}

ShipFactory::ShipFactory(Context* context) :
	LogicComponent(context)
{
	creationtimer_.Resize(Mob::MobType::MT_Count);

	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

bool ShipFactory::IsShipOnProduce(U32 type) const
{
	if (processShips_.Contains(type))
		return true;

	return false;
}

void ShipFactory::AddProduceShip(U32 typeNum)
{
	processShips_.Insert(typeNum);
	MarkNetworkUpdate();
}

void ShipFactory::SetProcessShips(const PODVector<U8>& value)
{
	if (value.Empty())
		return;

	MemoryBuffer buffer(value);
	while (!buffer.IsEof())
	{
		U32 shipType = buffer.ReadUInt();
		processShips_.Insert(shipType);
	}

	MarkNetworkUpdate();
}

PODVector<U8> ShipFactory::GetProcessShips() const
{
	VectorBuffer ret;

	for (auto shipIt = processShips_.Begin(); shipIt != processShips_.End(); ++shipIt)
		ret.WriteUInt(*shipIt);

	return ret.GetBuffer();
}

void ShipFactory::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	for (auto shipIt = processShips_.Begin(); shipIt != processShips_.End(); ++shipIt)
	{
		U32 typeNumber = *shipIt;

		if (creationtimer_[typeNumber].GetMSec(false) > creationTime_)
			creationtimer_[typeNumber].Reset();
		else
			continue;

		Node* ownerNode = LGeneralMethods::GetOwnerNode(node_);
		if (!ownerNode)
			continue;

		LResourceManager* resourceManager = ownerNode->GetComponent<LResourceManager>();
		if (!resourceManager || !resourceManager->ChangeResourceValue(LGameResource::ResourceType::Metals, createMobCost_))
			continue;

		PrefabsStorage* prefabsStorage = GetSubsystem<PrefabsStorage>();
		Node* mobNode = prefabsStorage->GetMob();
		if (!mobNode)
			continue;

		ownerNode->AddChild(mobNode);

		Owner* owner = mobNode->CreateComponent<Owner>();
		owner->SetOwnerID(ownerNode->GetID());

		LDataManager* dmanager = node_->GetParentComponent<LDataManager>(true);
		Mob* mob = mobNode->GetComponent<Mob>();
		if (mob && dmanager)
		{
			mob->Create(typeNumber, dmanager->GetTeamNumber());
			//equipment->AddElementToModule(Modules::Module::M_SecondWeaponPlace, TEquipment::ET_HyperDrive, "Chemical");
		}
	}
}
