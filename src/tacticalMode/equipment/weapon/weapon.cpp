#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>

#include "tacticalMode/equipment/tEquipment.h"
#include "tacticalMode/equipment/weapon/machineGun.h"

#include "tacticalMode/equipment/weapon/weapon.h"

using namespace Urho3D;

void Weapon::RegisterObject(Context* context)
{
	context->RegisterFactory<Weapon>();

	URHO3D_ATTRIBUTE("damage", F32, damage_, 0, AM_DEFAULT);

	URHO3D_ATTRIBUTE("currentHeat", F32, currentHeat_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("overheat", bool, overheat_, false, AM_DEFAULT);

	URHO3D_ATTRIBUTE("heatPerShot", F32, heatPerShot_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("heatReducePerSecond", F32, heatReducePerSecond_, 0, AM_DEFAULT);

	URHO3D_ATTRIBUTE("reloadingTime", F32, reloadingTime_, 0, AM_DEFAULT);
}

Weapon::Weapon(Context* context) :
	LogicComponent(context),
	damage_(0.f),
	currentHeat_(0.f),
	overheat_(false),
	heatPerShot_(0.f),
	heatReducePerSecond_(0.f),
	reloadingTime_(0)
{
	damage_ = 0.f;
}

Weapon::~Weapon()
{
}

bool Weapon::Shoot(Vector2 position)
{
	TEquipment* equipment = node_->GetComponent<TEquipment>();
	if (!equipment || !equipment->Use() || overheat_ || reloadingTimer_.GetMSec(false) < reloadingTime_)
		return false;

	WeaponInterface* weaponInterface = node_->GetDerivedComponent<WeaponInterface>();
	if (!weaponInterface)
		return false;

	weaponInterface->Shoot(position);

	currentHeat_ += heatPerShot_;
	reloadingTimer_.Reset();

	MarkNetworkUpdate();
	return true;
}

F32 Weapon::GetDamage() const
{
	return damage_;
}

void Weapon::Update(F32 timeStep)
{
	if (currentHeat_ >= 1)
	{
		overheat_ = true;
	}

	currentHeat_ -= (timeStep * heatReducePerSecond_);

	if (currentHeat_ <= 0)
	{
		currentHeat_ = 0;
		overheat_ = false;
	}
}
