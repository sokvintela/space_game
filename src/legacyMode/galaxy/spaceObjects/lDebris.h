#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class LDebris : public LogicComponent
{
	URHO3D_OBJECT(LDebris, LogicComponent);

public:

	static void RegisterObject(Context* context);
	LDebris(Context* context);
	~LDebris();

	void Create(Vector2 direction);

	F32 Getmetals() const;

private:
	Vector2 direction_;
	F32 speed_;
	F32 metals_;

	virtual void Update(F32 timeStep);
};