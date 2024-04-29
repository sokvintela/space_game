#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class Weapon : public LogicComponent
{
	URHO3D_OBJECT(Weapon, LogicComponent);

public:

	static void RegisterObject(Context* context);
	Weapon(Context* context);
	~Weapon();

	F32 GetDamage() const;

	bool Shoot(Vector2 position);

	//void CreateManagedFireBall();

protected:
	F32 damage_;

	F32 currentHeat_;
	bool overheat_;

	F32 heatPerShot_;
	F32 heatReducePerSecond_;

	Timer reloadingTimer_;
	F32 reloadingTime_;

	virtual void Update(F32 timeStep);
};
