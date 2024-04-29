#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class TPlanet : public LogicComponent
{
	URHO3D_OBJECT(TPlanet, LogicComponent);

public:
	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }
	TPlanet(Context* context);

	void Create(F32 radius, F32 angle, S32 teamNumber);

private:
	void HandleObjectDestroyed(StringHash eventType, VariantMap& eventData);
};
