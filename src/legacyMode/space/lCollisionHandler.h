#pragma once

#include <Urho3D/Core/Object.h>

using namespace Urho3D;

class LCollisionHandler : public Object
{
	URHO3D_OBJECT(LCollisionHandler, Object);

public:
	LCollisionHandler(Context* context);
	~LCollisionHandler();

	void HandleCollision(StringHash eventType, VariantMap & eventData);
};
