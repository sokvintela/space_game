#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class SelfDestroyingTimer : public LogicComponent
{
	URHO3D_OBJECT(SelfDestroyingTimer, LogicComponent);

public:
	static void RegisterObject(Context* context);
	SelfDestroyingTimer(Context* context);

	void SetLifeTime(F32 lifeTime);

private:
	F32 lifeTime_;
	Timer timer_;

	virtual void Update(F32 timeStep);
};
