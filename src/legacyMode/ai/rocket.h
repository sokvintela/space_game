#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class Rocket : public LogicComponent
{
	URHO3D_OBJECT(Rocket, LogicComponent);

public:

	static void RegisterObject(Context* context);
	Rocket(Context* context);
	~Rocket() = default;

	F32 GetSpeed() const;

private:
	F32 speed_;
};
