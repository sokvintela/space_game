#pragma once

#include <Urho3D/Core/Object.h>

using namespace Urho3D;

class TCollisionHandler : public Object
{
	URHO3D_OBJECT(TCollisionHandler, Object);

public:
	TCollisionHandler(Context* context);
	~TCollisionHandler();

	void HandleCollision(StringHash eventType, VariantMap & eventData);
};
