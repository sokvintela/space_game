#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"
#include "legacyMode/equipment/modules.h"

using namespace Urho3D;

class LSpaceShip : public LogicComponent
{
	URHO3D_OBJECT(LSpaceShip, LogicComponent);

public:
	static void RegisterObject(Context* context);
	LSpaceShip(Context* context);

	void Shoot(Modules::Module side, Vector2 mousePositionXY) const;
};
