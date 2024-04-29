#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"
#include "tacticalMode/managers/tEquipmentManager.h"

using namespace Urho3D;

class TSpaceShip : public LogicComponent
{
	URHO3D_OBJECT(TSpaceShip, LogicComponent);

public:
	static void RegisterObject(Context* context);
	TSpaceShip(Context* context);

	void Shoot(TEquipmentManager::Module side, Vector2 mousePositionXY) const;
};
