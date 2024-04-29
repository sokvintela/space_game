#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class RepairDrone : public LogicComponent
{
	URHO3D_OBJECT(RepairDrone, LogicComponent);

public:
	static void RegisterObject(Context* context);
	RepairDrone(Context* context);

private:
	virtual void Update(F32 timeStep);
};
