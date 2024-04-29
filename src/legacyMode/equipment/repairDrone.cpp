#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/spaceship/lDurability.h"
#include "legacyMode/equipment/lEquipment.h"

#include "legacyMode/equipment/repairDrone.h"

using namespace Urho3D;

void RepairDrone::RegisterObject(Context* context)
{
	context->RegisterFactory<RepairDrone>();
}

RepairDrone::RepairDrone(Context* context) :
	LogicComponent(context)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

void RepairDrone::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	LEquipmentManager* eManager = node_->GetParentComponent<LEquipmentManager>(true);
	if (!eManager)
		return;

	LDurability* shipDurability = node_->GetParentComponent<LDurability>(true);
	if (!shipDurability)
		return;

	LEquipment* equipment = node_->GetComponent<LEquipment>();
	if (equipment && equipment->Use())
		shipDurability->ChangeDurability(equipment->GetUseCost());
}
