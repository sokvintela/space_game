#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class CelestialBody : public LogicComponent
{
	URHO3D_OBJECT(CelestialBody, LogicComponent);

public:
	static void RegisterObject(Context* context);
	CelestialBody(Context* context);

	void SetAngle(F32 angle);
	void SetRadius(F32 radius);
	void SetAngularSpped(F32 angularSpeed);

private:
	F32 orbitRadius_;
	F32 angle_;
	F32 angleStep_;
	F32 angularSpeed_;

	virtual void Update(F32 timeStep);
};
