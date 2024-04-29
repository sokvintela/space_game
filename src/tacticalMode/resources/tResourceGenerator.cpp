#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "tacticalMode/resources/tResourceTransmitter.h"
#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/managers/tDataManager.h"

#include "tacticalMode/resources/tResourceGenerator.h"

using namespace Urho3D;

void TResourceGenerator::RegisterObject(Context * context)
{
	context->RegisterFactory<TResourceGenerator>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, tResourceTypeNames, TGameResource::ResourceType::Count, AM_DEFAULT);
}

TResourceGenerator::TResourceGenerator(Context* context) :
	LogicComponent(context),
	type_(TGameResource::ResourceType::Count)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

bool TResourceGenerator::AddOnGenerate(TGameResource::ResourceType resourceType)
{
	if (type_ == resourceType)
		return false;

	switch (resourceType)
	{
		case TGameResource::ResourceType::Metals:
		{
			break;
		}
		case TGameResource::ResourceType::Energy:
		{
			TResourceTransmitter* transmitter = node_->CreateComponent<TResourceTransmitter>();
			transmitter->AddOnTransmit(resourceType);

			break;
		}
	}

	type_ = resourceType;
	MarkNetworkUpdate();
	return true;
}

void TResourceGenerator::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	TResourceManager* rManager = node_->GetParentComponent<TResourceManager>(true);
	if (!rManager)
		return;

	TDataManager* dManager = rManager->GetNode()->GetComponent<TDataManager>();
	if (dManager)
		rManager->ChangeResourceValue(type_, dManager->GetObjectEfficiency() * timeStep);
}
