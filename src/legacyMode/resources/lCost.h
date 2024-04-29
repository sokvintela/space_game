#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "legacyMode/resources/lGameResource.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class LCost : public LogicComponent
{
	URHO3D_OBJECT(LCost, LogicComponent);

public:
	static void RegisterObject(Context* context);
	LCost(Context* context);
	virtual ~LCost();

	LGameResource::ResourceType GetCostType() const;
	F32 GetCreateCost() const;
	F32 GetImproveCost() const;

private:
	LGameResource::ResourceType type_;

	F32 createCost_;
	F32 improveCost_;
};
