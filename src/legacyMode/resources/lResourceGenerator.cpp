#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "legacyMode/resources/lResourceTransmitter.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/managers/lDataManager.h"

#include "legacyMode/resources/lResourceGenerator.h"

using namespace Urho3D;

void LResourceGenerator::RegisterObject(Context * context)
{
	context->RegisterFactory<LResourceGenerator>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, lResourceTypeNames, LGameResource::ResourceType::Count, AM_DEFAULT);
}

LResourceGenerator::LResourceGenerator(Context* context) :
	LogicComponent(context),
	type_(LGameResource::ResourceType::Count)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

bool LResourceGenerator::AddOnGenerate(LGameResource::ResourceType resourceType)
{
	if (type_ == resourceType)
		return false;

	switch (resourceType)
	{
	case LGameResource::ResourceType::Knowledge:
	{
		LResourceTransmitter* transmitter = node_->CreateComponent<LResourceTransmitter>();
		transmitter->AddOnTransmit(resourceType);
		break;
	}
	case LGameResource::ResourceType::Metals:
	{
		break;
	}
	case LGameResource::ResourceType::Energy:
	{
		LResourceTransmitter* transmitter = node_->CreateComponent<LResourceTransmitter>();
		transmitter->AddOnTransmit(resourceType);

		break;
	}
	}

	type_ = resourceType;
	MarkNetworkUpdate();
	return true;
}

void LResourceGenerator::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	LResourceManager* rManager = node_->GetParentComponent<LResourceManager>(true);
	LDataManager* dManager = node_->GetParentComponent<LDataManager>(true);
	if (rManager && dManager)
		rManager->ChangeResourceValue(type_, dManager->GetObjectEfficiency() * timeStep);
}
