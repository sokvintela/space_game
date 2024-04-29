#pragma once

#include "tacticalMode/equipment/weapon/weaponInterface.h"

#include "utility/simpleTypes.h"

using namespace Urho3D;

class Laser : public WeaponInterface
{
	URHO3D_OBJECT(Laser, WeaponInterface);

public:
	static void RegisterObject(Context* context);
	Laser(Context* context);
	~Laser();

	F32 GetAttackSpeed() const override;
	void Shoot(Vector2 position) override;

private:
	static F32 attackSpeed_;
};
