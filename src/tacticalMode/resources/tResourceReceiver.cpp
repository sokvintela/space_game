#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "tacticalMode/resources/tResourceReceiver.h"

using namespace Urho3D;

void TResourceReceiver::RegisterObject(Context * context)
{
	context->RegisterFactory<TResourceReceiver>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, tResourceTypeNames, TGameResource::ResourceType::Count, AM_DEFAULT);
}

TResourceReceiver::TResourceReceiver(Context* context) :
	LogicComponent(context),
	type_(TGameResource::ResourceType::Count)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

bool TResourceReceiver::AddOnReceive(TGameResource::ResourceType type)
{
	if (type_ == type)
		return false;

	type_ = type;
	MarkNetworkUpdate();
	return true;
}
