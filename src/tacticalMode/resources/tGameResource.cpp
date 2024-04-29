#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Resource/Localization.h>

#include "tacticalMode/resources/tGameResource.h"

using namespace Urho3D;

void TGameResource::RegisterObject(Context * context)
{
	context->RegisterFactory<TGameResource>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, tResourceTypeNames, ResourceType::Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("value", F32, value_, 0.f, AM_DEFAULT);
}

TGameResource::TGameResource(Context* context) :
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

bool TGameResource::AddResource(ResourceType type, F32 startValue)
{
	if (type_ == type)
		return false;

	type_ = type;
	value_ = startValue;
	MarkNetworkUpdate();
	return true;
}

String TGameResource::GetResourceName() const
{
	String name = tResourceTypeNames[type_];
	Localization* l10n = GetSubsystem<Localization>();
	String localizedName = l10n->Get(name);

	return localizedName;
}

F32 TGameResource::GetResourceValue() const
{
	return value_;
}

bool TGameResource::SetResourceValue(F32 value)
{
	value_ = value;
	MarkNetworkUpdate();
	return true;
}

bool TGameResource::ChangeResourceValue(F32 value)
{
	value_ += value;
	MarkNetworkUpdate();
	return true;
}

void TGameResource::Update(F32 timeStep)
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
