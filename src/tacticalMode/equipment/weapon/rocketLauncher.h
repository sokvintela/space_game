#pragma once

#include "tacticalMode/equipment/weapon/weaponInterface.h"

#include "utility/simpleTypes.h"

using namespace Urho3D;

class RocketLauncher : public WeaponInterface
{
	URHO3D_OBJECT(RocketLauncher, WeaponInterface);

public:
	static void RegisterObject(Context* context);
	RocketLauncher(Context* context);
	~RocketLauncher();

	F32 GetAttackSpeed() const override;
	void Shoot(Vector2 position) override;

private:
	F32 rocketSpeed_;
};
