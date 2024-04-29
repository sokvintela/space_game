#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

class WeaponInterface : public LogicComponent
{
	URHO3D_OBJECT(WeaponInterface, LogicComponent);

public:
	WeaponInterface(Context* context) : LogicComponent(context) { };
	virtual ~WeaponInterface() = default;

	virtual F32 GetAttackSpeed() const = 0;

	virtual void Shoot(Vector2 position) = 0;
};
