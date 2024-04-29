#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "legacyMode/resources/lResourceReceiver.h"

using namespace Urho3D;

void LResourceReceiver::RegisterObject(Context * context)
{
	context->RegisterFactory<LResourceReceiver>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, lResourceTypeNames, LGameResource::ResourceType::Count, AM_DEFAULT);
}

LResourceReceiver::LResourceReceiver(Context* context) :
	LogicComponent(context),
	type_(LGameResource::ResourceType::Count)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

bool LResourceReceiver::AddOnReceive(LGameResource::ResourceType type)
{
	if (type_ == type)
		return false;

	type_ = type;
	MarkNetworkUpdate();
	return true;
}
