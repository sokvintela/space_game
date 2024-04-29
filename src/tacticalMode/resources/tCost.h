#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "tacticalMode/resources/tGameResource.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class TCost : public LogicComponent
{
	URHO3D_OBJECT(TCost, LogicComponent);

public:
	static void RegisterObject(Context* context);
	TCost(Context* context);
	virtual ~TCost();

	TGameResource::ResourceType GetCostType() const;
	F32 GetCreateCost() const;
	F32 GetImproveCost() const;

private:
	TGameResource::ResourceType type_;

	F32 createCost_;
	F32 improveCost_;
};
