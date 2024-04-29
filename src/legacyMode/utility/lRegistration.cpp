#include <Urho3D/Core/Context.h>

#include "legacyMode/ai/researchProbe.h"
#include "legacyMode/ai/rocket.h"
#include "legacyMode/buildings/lBuilding.h"
#include "legacyMode/buildings/shipFactory.h"
#include "legacyMode/equipment/crystalDetector.h"
#include "legacyMode/equipment/hyperDrive.h"
#include "legacyMode/equipment/lEquipment.h"
#include "legacyMode/equipment/repairDrone.h"
#include "legacyMode/galaxy/galaxy.h"
#include "legacyMode/galaxy/lStarSystem.h"
#include "legacyMode/galaxy/spaceObjects/celestialBody.h"
#include "legacyMode/galaxy/spaceObjects/crystal.h"
#include "legacyMode/galaxy/spaceObjects/lAsteroid.h"
#include "legacyMode/galaxy/spaceObjects/lDebris.h"
#include "legacyMode/galaxy/spaceObjects/lPlanet.h"
#include "legacyMode/galaxy/spaceObjects/lStar.h"
#include "legacyMode/galaxy/spaceObjects/pulsar.h"
#include "legacyMode/managers/lBuildingsManager.h"
#include "legacyMode/managers/lDataManager.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/playerLogic/lBaseShip.h"
#include "legacyMode/playerLogic/lControlObject.h"
#include "legacyMode/playerLogic/lResearchTree.h"
#include "legacyMode/playerLogic/lTeam.h"
#include "legacyMode/playerLogic/owner.h"
#include "legacyMode/resources/lCost.h"
#include "legacyMode/resources/lGameResource.h"
#include "legacyMode/resources/lResourceGenerator.h"
#include "legacyMode/resources/lResourceTransmitter.h"
#include "legacyMode/resources/lResourceReceiver.h"
#include "legacyMode/spaceship/lDurability.h"
#include "legacyMode/spaceship/lEnergyShield.h"
#include "legacyMode/spaceship/lSpaceship.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/utility/lSharedData.h"

#include "legacyMode/utility/lRegistration.h"

void LRegistration::RegisterGameObjects(Context* context)
{
	ResearchProbe::RegisterObject(context);
	Rocket::RegisterObject(context);

	// buildings
	LBuilding::RegisterObject(context);
	ShipFactory::RegisterObject(context);

	// equipment
	LEquipment::RegisterObject(context);

	CrystalDetector::RegisterObject(context);
	HyperDrive::RegisterObject(context);
	RepairDrone::RegisterObject(context);

	// galaxy
	Galaxy::RegisterObject(context);
	LStarSystem::RegisterObject(context);

	// space objects
	CelestialBody::RegisterObject(context);
	Crystal::RegisterObject(context);
	LAsteroid::RegisterObject(context);
	LDebris::RegisterObject(context);
	LPlanet::RegisterObject(context);
	LStar::RegisterObject(context);
	Pulsar::RegisterObject(context);

	// maangers
	LBuildingsManager::RegisterObject(context);
	LDataManager::RegisterObject(context);
	LEquipmentManager::RegisterObject(context);
	LResourceManager::RegisterObject(context);

	// player logic
	LBaseShip::RegisterObject(context);
	LControlObject::RegisterObject(context);
	LResearchTree::RegisterObject(context);
	LTeam::RegisterObject(context);
	Owner::RegisterObject(context);
	
	// resources
	LCost::RegisterObject(context);
	LGameResource::RegisterObject(context);
	LResourceGenerator::RegisterObject(context);
	LResourceTransmitter::RegisterObject(context);
	LResourceReceiver::RegisterObject(context);

	// spaceship
	LDurability::RegisterObject(context);
	LEnergyShield::RegisterObject(context);
	LSpaceShip::RegisterObject(context);
}
