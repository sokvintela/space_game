#include <Urho3D/Core/Context.h>

#include "tacticalMode/equipment/weapon/weapon.h"
#include "tacticalMode/managers/tEquipmentManager.h"

#include "tacticalMode/spaceship/tSpaceship.h"

using namespace Urho3D;

void TSpaceShip::RegisterObject(Context* context)
{
	context->RegisterFactory<TSpaceShip>();
}

TSpaceShip::TSpaceShip(Context* context)
	: LogicComponent(context)
{
}

void TSpaceShip::Shoot(TEquipmentManager::Module weaponModule, Vector2 mousePositionXY) const
{
	TEquipmentManager* manager = node_->GetComponent<TEquipmentManager>();
	if (!manager)
		return;

	Weapon* weaponComponentToFire = manager->GetElement<Weapon>(weaponModule);
	if (weaponComponentToFire)
		weaponComponentToFire->Shoot(mousePositionXY);
}
