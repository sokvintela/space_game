#pragma once

#include "tacticalMode/equipment/weapon/weaponInterface.h"

#include "utility/simpleTypes.h"

using namespace Urho3D;

class MachineGun : public WeaponInterface
{
	URHO3D_OBJECT(MachineGun, WeaponInterface);

public:
	static void RegisterObject(Context* context);
	MachineGun(Context* context);
	~MachineGun();

	F32 GetAttackSpeed() const override;
	void Shoot(Vector2 position) override;

private:
	F32 bulletSpeed_;
};
