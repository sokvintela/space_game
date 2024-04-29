#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

class GravityAffected : public LogicComponent
{
	URHO3D_OBJECT(GravityAffected, LogicComponent);

public:

	static void RegisterObject(Context* context);
	GravityAffected(Context* context);
	~GravityAffected();
};
