#include <Urho3D/Core/Context.h>

#include "ai/AIBase.h"
#include "equipment/shipEngine.h"
#include "equipment/energyFieldGenerator.h"
#include "tacticalMode/resources/tCost.h"
#include "tacticalMode/resources/tGameResource.h"
#include "tacticalMode/resources/tResourceReceiver.h"
#include "utility/selfDestroyingTimer.h"

#include "tacticalMode/ai/mob.h"
#include "tacticalMode/equipment/tEquipment.h"
#include "tacticalMode/equipment/weapon/weapon.h"
#include "tacticalMode/equipment/weapon/weaponInterface.h"
#include "tacticalMode/equipment/weapon/machineGun.h"
#include "tacticalMode/equipment/weapon/laser.h"
#include "tacticalMode/equipment/weapon/rocketLauncher.h"
#include "tacticalMode/managers/tDataManager.h"
#include "tacticalMode/managers/tEquipmentManager.h"
#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/playerLogic/tControlObject.h"
#include "tacticalMode/playerLogic/tTeam.h"
#include "tacticalMode/resources/tResourceGenerator.h"
#include "tacticalMode/resources/tResourceTransmitter.h"
#include "tacticalMode/spaceship/tDurability.h"
#include "tacticalMode/spaceship/tEnergyShield.h"
#include "tacticalMode/spaceship/tSpaceship.h"
#include "tacticalMode/starSystem/gravity.h"
#include "tacticalMode/starSystem/gravityAffected.h"
#include "tacticalMode/starSystem/tAsteroid.h"
#include "tacticalMode/starSystem/tBaseShip.h"
#include "tacticalMode/starSystem/tDebris.h"
#include "tacticalMode/starSystem/tPlanet.h"
#include "tacticalMode/starSystem/tStar.h"
#include "tacticalMode/starSystem/tStarSystem.h"
#include "tacticalMode/starSystem/tBaseShip.h"

#include "utility/registration.h"

void Registration::RegisterGameObjects(Context* context)
{
	// ai
	AIBase::RegisterObject(context);

	// equipment
	ShipEngine::RegisterObject(context);
	EnergyFieldGenerator::RegisterObject(context);

	// resources
	TCost::RegisterObject(context);
	TGameResource::RegisterObject(context);
	TResourceReceiver::RegisterObject(context);

	// utility
	SelfDestroyingTimer::RegisterObject(context);

	// TACTICAL MODE
	// ai
	Mob::RegisterObject(context);

	// equipment
	TEquipment::RegisterObject(context);

	// weapon
	Weapon::RegisterObject(context);
	MachineGun::RegisterObject(context);
	Laser::RegisterObject(context);
	RocketLauncher::RegisterObject(context);

	// managers
	TDataManager::RegisterObject(context);
	TEquipmentManager::RegisterObject(context);
	TResourceManager::RegisterObject(context);

	// plater logic
	TControlObject::RegisterObject(context);
	TTeam::RegisterObject(context);
	TBaseShip::RegisterObject(context);

	// resources
	TResourceGenerator::RegisterObject(context);
	TResourceTransmitter::RegisterObject(context);

	// spaceship
	TDurability::RegisterObject(context);
	TEnergyShield::RegisterObject(context);
	TSpaceShip::RegisterObject(context);

	// star system
	Gravity::RegisterObject(context);
	GravityAffected::RegisterObject(context);
	TAsteroid::RegisterObject(context);
	TDebris::RegisterObject(context);
	TPlanet::RegisterObject(context);
	TStar::RegisterObject(context);
	TStarSystem::RegisterObject(context);
}
