#include <Urho3D/Core/Context.h>

#include "tacticalMode/equipment/weapon/weapon.h"
#include "legacyMode/managers/lEquipmentManager.h"

#include "legacyMode/spaceship/lSpaceship.h"

using namespace Urho3D;

void LSpaceShip::RegisterObject(Context* context)
{
	context->RegisterFactory<LSpaceShip>();
}

LSpaceShip::LSpaceShip(Context* context)
	: LogicComponent(context)
{
}

void LSpaceShip::Shoot(Modules::Module side, Vector2 mousePositionXY) const
{
	LEquipmentManager* manager = node_->GetComponent<LEquipmentManager>();
	if (!manager)
		return;

	Weapon* weaponComponentToFire = manager->GetWeapon(side);
	if (weaponComponentToFire)
		weaponComponentToFire->Shoot(mousePositionXY);
}
