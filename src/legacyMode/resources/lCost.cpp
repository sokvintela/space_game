#include <Urho3D/Core/Context.h>

#include "legacyMode/resources/lCost.h"

void LCost::RegisterObject(Context* context)
{
	context->RegisterFactory<LCost>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, lResourceTypeNames, LGameResource::ResourceType::Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("createCost", F32 , createCost_, 0.f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("improveCost", F32, improveCost_, 0.f, AM_DEFAULT);
}

LCost::LCost(Context * context) :
	LogicComponent(context),
	type_(LGameResource::ResourceType::Count),
	createCost_(0.f),
	improveCost_(0.f)
{
}

LCost::~LCost()
{
}

LGameResource::ResourceType LCost::GetCostType() const
{
	return type_;
}

F32 LCost::GetCreateCost() const
{
	return createCost_;
}

F32 LCost::GetImproveCost() const
{
	return improveCost_;
}
