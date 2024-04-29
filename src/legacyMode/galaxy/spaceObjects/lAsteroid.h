#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class LAsteroid : public LogicComponent
{
	URHO3D_OBJECT(LAsteroid, LogicComponent);

public:

	static void RegisterObject(Context* context);
	LAsteroid(Context* context);
	~LAsteroid();

	void Create(Vector2 direction);

private:
	Vector2 direction_;
	F32 speed_;

	virtual void Update(F32 timeStep);
};