#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Scene/Node.h>

#include "equipment/shipEngine.h"
#include "legacyMode/galaxy/lStarSystem.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "tacticalMode/equipment/weapon/weapon.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/managers/lBuildingsManager.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/galaxy/spaceObjects/crystal.h"
#include "legacyMode/managers/lDataManager.h"
#include "prefabs/prefabsStorage.h"
#include "legacyMode/utility/lSharedData.h"
#include "legacyMode/equipment/crystalDetector.h"
#include "legacyMode/equipment/lEquipment.h"

#include "legacyMode/ai/lAINodeLogic.h"

namespace AI
{
namespace Nodes
{
// follow
std::unique_ptr<AINode> FollowZeroPlayer::Clone() const
{
	auto newNode = std::make_unique<FollowZeroPlayer>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus FollowZeroPlayer::Process(F32 timeStep)
{
	if (LGeneralMethods::MoveObject(aiElement_, LGeneralMethods::GetMyControlNode(aiElement_)))
		return NodeReturnStatus::Success;

	return NodeReturnStatus::Failure;
}

std::unique_ptr<AINode> FollowOwner::Clone() const
{
	auto newNode = std::make_unique<FollowOwner>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus FollowOwner::Process(F32 timeStep)
{
	Node* ownerNode = LGeneralMethods::GetOwnerNode(aiElement_);
	if (!ownerNode)
		return NodeReturnStatus::Failure;

	if (LGeneralMethods::MoveObject(LGeneralMethods::GetOwnerNode(aiElement_), ownerNode->GetID(), 1.0f))
		return NodeReturnStatus::Success;

	return NodeReturnStatus::Failure;
}

std::unique_ptr<AINode> FollowTarget::Clone() const
{
	auto newNode = std::make_unique<FollowTarget>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus FollowTarget::Process(F32 timeStep)
{
	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager)
		return NodeReturnStatus::Failure;

	bool moveObject = LGeneralMethods::MoveObject(aiElement_, dManager->GetTargetID());

	if (moveObject)
		return NodeReturnStatus::Success;

	return NodeReturnStatus::Failure;
}

std::unique_ptr<AINode> FlyToClosestSolidPlanet::Clone() const
{
	auto newNode = std::make_unique<FlyToClosestSolidPlanet>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus FlyToClosestSolidPlanet::Process(F32 timeStep)
{
	Vector2 direction = LGeneralMethods::GetClosestPlanetPosition(aiElement_, LPlanet::PlanetType::PT_Solid) - aiElement_->GetWorldPosition2D();
	if (direction.Length() < 5.0f)
		return NodeReturnStatus::Success;

	if (LGeneralMethods::MoveObject(aiElement_, direction))
		return NodeReturnStatus::Running;

	return NodeReturnStatus::Failure;
}

std::unique_ptr<AINode> FlyToTargetPlanet::Clone() const
{
	auto newNode = std::make_unique<FlyToTargetPlanet>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus FlyToTargetPlanet::Process(F32 timeStep)
{
	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager)
		return NodeReturnStatus::Failure;

	Node* planetNode = LGeneralMethods::GetObjectNode(aiElement_, dManager->GetTargetID());
	if (!planetNode)
		return NodeReturnStatus::Failure;

	Vector2 direction = planetNode->GetWorldPosition2D() - aiElement_->GetWorldPosition2D();
	if (direction.Length() < 5.0f)
		return NodeReturnStatus::Success;

	if (LGeneralMethods::MoveObject(aiElement_, direction))
		return NodeReturnStatus::Running;

	return NodeReturnStatus::Failure;
}

std::unique_ptr<AINode> FlyToCrystal::Clone() const
{
	auto newNode = std::make_unique<FlyToCrystal>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus FlyToCrystal::Process(F32 timeStep)
{
	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager)
		return NodeReturnStatus::Failure;

	LStarSystem* system = LGeneralMethods::GetStarSystem(aiElement_, dManager->GetCurrentStarSystemID());
	if (!system)
		return NodeReturnStatus::Failure;

	Crystal* crystal = system->GetComponent<Crystal>();
	if (!crystal)
		return NodeReturnStatus::Failure;

	Vector2 direction = crystal->GetNode()->GetWorldPosition2D() - aiElement_->GetWorldPosition2D();
	if (direction.Length() < 1.0f)
		return NodeReturnStatus::Success;

	if (LGeneralMethods::MoveObject(aiElement_, direction))
		return NodeReturnStatus::Running;

	return NodeReturnStatus::Failure;
}

float SpiralSearch::spiralCoeff_ = 100.0f;

SpiralSearch::SpiralSearch()
	: absoluteAngle_(0.f)
{
}

std::unique_ptr<AINode> SpiralSearch::Clone() const
{
	auto newNode = std::make_unique<SpiralSearch>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus SpiralSearch::Process(F32 timeStep)
{
	LEquipmentManager* manager = aiElement_->GetComponent<LEquipmentManager>();
	Owner* owner = aiElement_->GetComponent<Owner>();
	if (!manager || !owner)
		return NodeReturnStatus::Failure;

	Node* startNode = LGeneralMethods::GetOwnerNode(aiElement_);
	ShipEngine* engine = manager->GetEquipmentItem<ShipEngine>();
	if (!engine || !startNode)
		return NodeReturnStatus::Failure;

	if (absoluteAngle_ == 0.0f)
		startingPosition_ = startNode->GetWorldPosition2D();

	F32 curRadius = 10.f * spiralCoeff_ * absoluteAngle_;
	if (absoluteAngle_ == 0.0f)
		absoluteAngle_ += timeStep;
	else
		absoluteAngle_ += engine->GetSpeed() * timeStep / curRadius;

	Vector2 endPosition = startingPosition_ + Vector2(curRadius * std::cos(absoluteAngle_), curRadius * std::sin(absoluteAngle_));
	Vector2 direction = endPosition - startingPosition_;

	if (LGeneralMethods::MoveObject(aiElement_, direction * 1000.f))
		return NodeReturnStatus::Success;

	return NodeReturnStatus::Failure;
}

// checks
std::unique_ptr<AINode> HasCrystalDetector::Clone() const
{
	auto newNode = std::make_unique<HasCrystalDetector>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus HasCrystalDetector::Process(F32 timeStep)
{
	LEquipmentManager* manager = aiElement_->GetComponent<LEquipmentManager>();
	if (manager && manager->GetEquipmentItem<CrystalDetector>())
		return NodeReturnStatus::Success;

	return NodeReturnStatus::Failure;
}

std::unique_ptr<AINode> HasResourceOnEFactoryAndCDetector::Clone() const
{
	auto newNode = std::make_unique<HasResourceOnEFactoryAndCDetector>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus HasResourceOnEFactoryAndCDetector::Process(F32 timeStep)
{
	auto costType = aiElement_->GetSubsystem<PrefabsStorage>()->GetBuildingCreateCost(LBuilding::TargetType::TT_Planet, "equipmentFactory").first_;
	F32 costValue = aiElement_->GetSubsystem<PrefabsStorage>()->GetBuildingCreateCost(LBuilding::TargetType::TT_Planet, "equipmentFactory").second_
		+ aiElement_->GetSubsystem<PrefabsStorage>()->GetLegacyEquipmentCreateCost(LEquipment::EquipmentType::ET_CrystalDetector, "CrystalDetector").second_;

	LResourceManager* manager = aiElement_->GetComponent<LResourceManager>();
	if (manager && manager->HasResource(costType, costValue))
		return NodeReturnStatus::Success;

	return NodeReturnStatus::Failure;
}

std::unique_ptr<AINode> HasResourceOnRadar::Clone() const
{
	auto newNode = std::make_unique<HasResourceOnRadar>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus HasResourceOnRadar::Process(F32 timeStep)
{
	auto radarCost = aiElement_->GetSubsystem<PrefabsStorage>()->GetBuildingCreateCost(LBuilding::TargetType::TT_Planet, "radar");
	LResourceManager* manager = aiElement_->GetComponent<LResourceManager>();
	if (manager && manager->HasResource(radarCost.first_, radarCost.second_))
		return NodeReturnStatus::Success;

	return NodeReturnStatus::Failure;
}

std::unique_ptr<AINode> HasEveryPanetImprovedRadar::Clone() const
{
	auto newNode = std::make_unique<HasEveryPanetImprovedRadar>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus HasEveryPanetImprovedRadar::Process(F32 timeStep)
{
	PODVector<Node*> planetNodes = LGeneralMethods::GetPlanetNodes(LGeneralMethods::GetCurrentStarSystemNode(aiElement_), LPlanet::PlanetType::PT_Solid);
	for (U32 i = 0; i < planetNodes.Size(); i++)
	{
		LBuildingsManager* bManager = planetNodes[i]->GetComponent<LBuildingsManager>();
		LResourceManager* rManager = planetNodes[i]->GetComponent<LResourceManager>();
		if (!bManager || !rManager)
			continue;

		if (!bManager->GetBuildingNode("radar") 
			|| !rManager->GetResourceDeviceNode(LGameResource::ResourceDevice::Transmitter, LGameResource::ResourceType::Energy))
			return NodeReturnStatus::Failure;
	}

	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> BetterRadarToGetEnergy::Clone() const
{
	auto newNode = std::make_unique<BetterRadarToGetEnergy>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus BetterRadarToGetEnergy::Process(F32 timeStep)
{
	String bType = "radar";

	F32 maxEnergy = 0.f;
	S32 planetID = -1;

	PODVector<Node*> planetNodes;
	planetNodes = LGeneralMethods::GetPlanetNodes(LGeneralMethods::GetCurrentStarSystemNode(aiElement_), LPlanet::PlanetType::PT_Solid);

	for (U32 i = 0; i < planetNodes.Size(); i++)
	{
		LResourceManager* rManager = LGeneralMethods::GetPlanetBuildingResourceManager(planetNodes[i], bType);
		LBuildingsManager* bManager = planetNodes[i]->GetComponent<LBuildingsManager>();
		if (!bManager || !bManager->GetBuilding(bType)
			|| !rManager || rManager->GetResourceDeviceNode(LGameResource::ResourceDevice::Transmitter, LGameResource::ResourceType::Energy))
			continue;

		float value = rManager->GetResourceValue(LGameResource::ResourceType::Energy);
		if (value > maxEnergy)
		{
			maxEnergy = value;
			planetID = planetNodes[i]->GetID();
		}
	}

	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager)
		return NodeReturnStatus::Failure;

	dManager->SetTargetID(planetID);
	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> PlanetWithoutRadar::Clone() const
{
	auto newNode = std::make_unique<PlanetWithoutRadar>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus PlanetWithoutRadar::Process(F32 timeStep)
{
	F32 minDistance = INFINITY;
	S32 planetID = -1;

	PODVector<Node*> planetNodes = LGeneralMethods::GetPlanetNodes(LGeneralMethods::GetCurrentStarSystemNode(aiElement_), LPlanet::PlanetType::PT_Solid);
	for (U32 i = 0; i < planetNodes.Size(); i++)
	{
		LBuildingsManager* buildingManager = planetNodes[i]->GetComponent<LBuildingsManager>();
		if (!buildingManager || buildingManager->GetBuilding("radar"))
			continue;

		float distance = (planetNodes[i]->GetWorldPosition2D() - aiElement_->GetWorldPosition2D()).Length();
		if (distance < minDistance)
		{
			minDistance = distance;
			planetID = planetNodes[i]->GetID();
		}
	}

	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager)
		return NodeReturnStatus::Failure;

	dManager->SetTargetID(planetID);
	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> PlanetWithoutDefenderPlatform::Clone() const
{
	auto newNode = std::make_unique<PlanetWithoutDefenderPlatform>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus PlanetWithoutDefenderPlatform::Process(F32 timeStep)
{
	F32 minDistance = INFINITY;
	S32 planetID = -1;

	PODVector<Node*> planetNodes = LGeneralMethods::GetPlanetNodes(LGeneralMethods::GetCurrentStarSystemNode(aiElement_), LPlanet::PlanetType::PT_Solid);
	for (U32 i = 0; i < planetNodes.Size(); i++)
	{
		LBuildingsManager* buildingManager = planetNodes[i]->GetComponent<LBuildingsManager>();
		if (!buildingManager || buildingManager->GetBuilding("defenderPlatform"))
			continue;

		float distance = (planetNodes[i]->GetWorldPosition2D() - aiElement_->GetWorldPosition2D()).Length();
		if (distance < minDistance)
		{
			minDistance = distance;
			planetID = planetNodes[i]->GetID();
		}
	}

	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager)
		return NodeReturnStatus::Failure;

	dManager->SetTargetID(planetID);
	return NodeReturnStatus::Success;
}

// planet
std::unique_ptr<AINode> BuildEquipmentFactory::Clone() const
{
	auto newNode = std::make_unique<BuildEquipmentFactory>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus BuildEquipmentFactory::Process(F32 timeStep)
{
	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager || dManager->GetTargetID() == -1)
		return NodeReturnStatus::Failure;

	VariantMap buildEventData;

	buildEventData[ObjectBuildEvent::PLAYER_ID] = aiElement_->GetID();
	buildEventData[ObjectBuildEvent::OBJECT_ID] = dManager->GetTargetID();
	buildEventData[ObjectBuildEvent::BUILDING_NAME] = "equipmentFactory";
	buildEventData[ObjectBuildEvent::TARGET_TYPE] = LBuilding::TargetType::TT_Planet;

	aiElement_->SendEvent(RE_OBJECT_BUILD, buildEventData);
	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> CreateCrystalDetector::Clone() const
{
	auto newNode = std::make_unique<CreateCrystalDetector>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus CreateCrystalDetector::Process(F32 timeStep)
{
	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager)
		return NodeReturnStatus::Failure;

	LBuildingsManager* buildingManager = LGeneralMethods::GetObjectManager<LBuildingsManager>(aiElement_, dManager->GetTargetID());
	if (!buildingManager || !buildingManager->GetBuilding("equipmentFactory"))
		return NodeReturnStatus::Failure;

	VariantMap createCrystalDetectorEventData;
	createCrystalDetectorEventData[CreateEquipmentEvent::PLAYER_ID] = aiElement_->GetID();
	createCrystalDetectorEventData[CreateEquipmentEvent::EQUIPMENT_TYPE] = LEquipment::EquipmentType::ET_CrystalDetector;
	createCrystalDetectorEventData[CreateEquipmentEvent::EQUIPMENT_NAME] = "CrystalDetector";

	aiElement_->SendEvent(RE_CREATE_EQUIPMENT, createCrystalDetectorEventData);
	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> BuildRadar::Clone() const
{
	auto newNode = std::make_unique<BuildRadar>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus BuildRadar::Process(F32 timeStep)
{
	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager || dManager->GetTargetID() == -1)
		return NodeReturnStatus::Failure;

	VariantMap buildEventData;
	buildEventData[ObjectBuildEvent::PLAYER_ID] = aiElement_->GetID();
	buildEventData[ObjectBuildEvent::OBJECT_ID] = dManager->GetTargetID();
	buildEventData[ObjectBuildEvent::BUILDING_NAME] = "radar";
	buildEventData[ObjectBuildEvent::TARGET_TYPE] = LBuilding::TargetType::TT_Planet;

	aiElement_->SendEvent(RE_OBJECT_BUILD, buildEventData);
	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> TakeRadarEnergy::Clone() const
{
	auto newNode = std::make_unique<TakeRadarEnergy>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus TakeRadarEnergy::Process(F32 timeStep)
{
	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager || dManager->GetTargetID() == -1)
		return NodeReturnStatus::Failure;

	VariantMap getRadarEnergyEventData;
	getRadarEnergyEventData[GetBuildingResourceEnergyEvent::PLAYER_ID] = aiElement_->GetID();
	getRadarEnergyEventData[GetBuildingResourceEnergyEvent::OBJECT_ID] = dManager->GetTargetID();
	getRadarEnergyEventData[GetBuildingResourceEnergyEvent::BUILDING_NAME] = "radar";
	getRadarEnergyEventData[GetBuildingResourceEnergyEvent::RESOURCE_TYPE] = LGameResource::ResourceType::Energy;

	aiElement_->SendEvent(RE_GET_BUILDING_RESOURCE, getRadarEnergyEventData);
	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> ImproveRadar::Clone() const
{
	auto newNode = std::make_unique<ImproveRadar>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus ImproveRadar::Process(F32 timeStep)
{
	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager || dManager->GetTargetID() == -1)
		return NodeReturnStatus::Failure;

	VariantMap improveRadarEventData;
	improveRadarEventData[ImproveBuildingEvent::OBJECT_ID] = dManager->GetTargetID();
	improveRadarEventData[ImproveBuildingEvent::PLAYER_ID] = aiElement_->GetID();
	improveRadarEventData[ImproveBuildingEvent::BUILDING_NAME] = "radar";

	aiElement_->SendEvent(RE_IMPROVE_BUILDING, improveRadarEventData);
	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> BuildDefenderPlatform::Clone() const
{
	auto newNode = std::make_unique<BuildDefenderPlatform>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus BuildDefenderPlatform::Process(F32 timeStep)
{
	LDataManager* dManager = aiElement_->GetComponent<LDataManager>();
	if (!dManager || dManager->GetTargetID() == -1)
		return NodeReturnStatus::Failure;

	VariantMap createDefenderPlatformEventData;
	createDefenderPlatformEventData[ObjectBuildEvent::OBJECT_ID] = dManager->GetTargetID();
	createDefenderPlatformEventData[ObjectBuildEvent::PLAYER_ID] = aiElement_->GetID();
	createDefenderPlatformEventData[ObjectBuildEvent::BUILDING_NAME] = "orbitalFactory";
	createDefenderPlatformEventData[ObjectBuildEvent::TARGET_TYPE] = LBuilding::TargetType::TT_Planet;

	aiElement_->SendEvent(RE_OBJECT_BUILD, createDefenderPlatformEventData);
	return NodeReturnStatus::Success;
}

// crystal
std::unique_ptr<AINode> UseCrystalDetector::Clone() const
{
	auto newNode = std::make_unique<UseCrystalDetector>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus UseCrystalDetector::Process(F32 timeStep)
{
	VariantMap crystalDetectorUseEventData;
	crystalDetectorUseEventData[CrystalDetectorUseEvent::PLAYER_ID] = aiElement_->GetID();

	aiElement_->GetSubsystem<LSharedData>()->SendGameEvent(RE_DETECTOR_USE, crystalDetectorUseEventData);
	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> FireIfEnemy::Clone() const
{
	auto newNode = std::make_unique<FireIfEnemy>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus FireIfEnemy::Process(F32 timeStep)
{
	S32 ownerTeamNumber = LGeneralMethods::GetOwnerTeamNumber(aiElement_);
	LEquipmentManager* myManager = aiElement_->GetComponent<LEquipmentManager>();
	if (!myManager)
		return NodeReturnStatus::Failure;

	PODVector<Node*> shipNodes = LGeneralMethods::GetShipNodes(aiElement_);
	for (U32 i = 0; i < shipNodes.Size(); i++)
	{
		S32 enemyOwnerTeamID = LGeneralMethods::GetOwnerTeamNumber(shipNodes[i]);
		if (ownerTeamNumber == enemyOwnerTeamID)
			continue;

		// TODO : galaxy Radius
		Vector2 enemyPos = shipNodes[i]->GetWorldPosition2D();
		if ((enemyPos - aiElement_->GetWorldPosition2D()).Length() < 50.f)
		{
			Weapon* weapon = myManager->GetWeapon(Modules::Module::M_FirstWeaponPlace);
			if (weapon)
				weapon->Shoot(enemyPos);

			weapon = myManager->GetWeapon(Modules::Module::M_SecondWeaponPlace);
			if (weapon)
				weapon->Shoot(enemyPos);
		}
	}

	return NodeReturnStatus::Success;
}

};
};
