#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class Gravity : public LogicComponent
{
	URHO3D_OBJECT(Gravity, LogicComponent);

public:

	static void RegisterObject(Context* context);
	Gravity(Context* context);
	~Gravity();

private:
	virtual void Update(F32 timeStep);
};
