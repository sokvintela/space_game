#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class LBaseShip : public LogicComponent
{
	URHO3D_OBJECT(LBaseShip, LogicComponent);

public:

	static void RegisterObject(Context* context);
	LBaseShip(Context* context);

	void Create();

private:
	void HandleObjectDestroyed(StringHash eventType, VariantMap& eventData);
};
