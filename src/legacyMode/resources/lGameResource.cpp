#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "legacyMode/resources/lGameResource.h"

using namespace Urho3D;

void LGameResource::RegisterObject(Context * context)
{
	context->RegisterFactory<LGameResource>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, lResourceTypeNames, ResourceType::Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("value", F32, value_, 0.f, AM_DEFAULT);
}

LGameResource::LGameResource(Context* context) :
	LogicComponent(context),
	type_(ResourceType::Count),
	value_(0.f)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

bool LGameResource::AddResource(ResourceType type, F32 startValue)
{
	if (type_ == type)
		return false;

	type_ = type;
	value_ = startValue;
	MarkNetworkUpdate();
	return true;
}

String LGameResource::GetResourceName() const
{
	if (type_ == ResourceType::Count)
		return "";

	return lResourceTypeNames[type_];
}

F32 LGameResource::GetResourceValue() const
{
	return value_;
}

bool LGameResource::SetResourceValue(F32 value)
{
	value_ = value;
	MarkNetworkUpdate();
	return true;
}

bool LGameResource::ChangeResourceValue(F32 value)
{
	value_ += value;
	MarkNetworkUpdate();
	return true;
}

void LGameResource::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	if (value_ < 0.f)
	{
		value_ = 0.f;
		MarkNetworkUpdate();
	}
}
