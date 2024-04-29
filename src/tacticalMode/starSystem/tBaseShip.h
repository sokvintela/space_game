#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class TBaseShip : public LogicComponent
{
	URHO3D_OBJECT(TBaseShip, LogicComponent);

public:

	static void RegisterObject(Context* context);
	TBaseShip(Context* context);

	void Create();

private:
	void HandleObjectDestroyed(StringHash eventType, VariantMap& eventData);
};
