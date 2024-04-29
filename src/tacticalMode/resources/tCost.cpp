#include <Urho3D/Core/Context.h>

#include "tacticalMode/resources/tCost.h"

void TCost::RegisterObject(Context* context)
{
	context->RegisterFactory<TCost>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, tResourceTypeNames, TGameResource::ResourceType::Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("createCost", F32 , createCost_, 0.f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("improveCost", F32, improveCost_, 0.f, AM_DEFAULT);
}

TCost::TCost(Context * context) :
	LogicComponent(context),
	type_(TGameResource::ResourceType::Count),
	createCost_(0.f),
	improveCost_(0.f)
{
}

TCost::~TCost()
{
}

TGameResource::ResourceType TCost::GetCostType() const
{
	return type_;
}

F32 TCost::GetCreateCost() const
{
	return createCost_;
}

F32 TCost::GetImproveCost() const
{
	return improveCost_;
}
