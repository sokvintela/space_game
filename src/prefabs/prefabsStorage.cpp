#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>

#include "legacyMode/ai/researchProbe.h"
#include "legacyMode/playerLogic/lResearchTree.h"
#include "legacyMode/resources/lCost.h"
#include "tacticalMode/resources/tCost.h"
#include "tacticalMode/ai/mob.h"
#include "legacyMode/buildings/lBuilding.h"
#include "legacyMode/equipment/lEquipment.h"

#include "prefabs/prefabsStorage.h"

using namespace Urho3D;

PrefabsStorage::PrefabsStorage(Context* context) :
	Object(context)
{
	Load();
}

void PrefabsStorage::Load()
{
	scene_ = new Scene(context_);

	equipment_.Resize(TEquipment::EquipmentType::ET_Count);

	// engines
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_Engine, "Prefabs/Engines/sun.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_Engine, "Prefabs/Engines/chemical.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_Engine, "Prefabs/Engines/ion.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_Engine, "Prefabs/Engines/plasma.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_Engine, "Prefabs/Engines/nuclear.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_Engine, "Prefabs/Engines/photon.xml");

	// weapon
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_Weapon, "Prefabs/Weapon/MachineGun/machineGun.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_Weapon, "Prefabs/Weapon/laser.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_Weapon, "Prefabs/Weapon/rocketLauncher.xml");

	// energy field generators
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_FieldGenerator, "Prefabs/EnergyFieldGenerators/sun.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_FieldGenerator, "Prefabs/EnergyFieldGenerators/chemical.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_FieldGenerator, "Prefabs/EnergyFieldGenerators/ion.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_FieldGenerator, "Prefabs/EnergyFieldGenerators/plasma.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_FieldGenerator, "Prefabs/EnergyFieldGenerators/nuclear.xml");
	LoadEquipmentFromXML(TEquipment::EquipmentType::ET_FieldGenerator, "Prefabs/EnergyFieldGenerators/photon.xml");

	// ships
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	XMLFile* mobXml = cache->GetResource<XMLFile>("Prefabs/Ships/Mob.xml");
	if (mobXml)
		mobNode_ = scene_->InstantiateXML(mobXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* baseShipXml = cache->GetResource<XMLFile>("Prefabs/Ships/BaseShip.xml");
	if (baseShipXml)
		baseShipNode_ = scene_->InstantiateXML(baseShipXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	// space objects
	XMLFile* starXml = cache->GetResource<XMLFile>("Prefabs/SpaceObjects/Star.xml");
	if (starXml)
		starNode_ = scene_->InstantiateXML(starXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* planetXml = cache->GetResource<XMLFile>("Prefabs/SpaceObjects/Planet.xml");
	if (planetXml)
		planetNode_ = scene_->InstantiateXML(planetXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* asteroidXml = cache->GetResource<XMLFile>("Prefabs/SpaceObjects/Asteroid.xml");
	if (asteroidXml)
		asteroidNode_ = scene_->InstantiateXML(asteroidXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* debrisXml = cache->GetResource<XMLFile>("Prefabs/SpaceObjects/Debris.xml");
	if (debrisXml)
		debrisNode_ = scene_->InstantiateXML(debrisXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	// player logic
	XMLFile* teamXml = cache->GetResource<XMLFile>("Prefabs/PlayersLogic/Team.xml");
	if (teamXml)
		teamNode_ = scene_->InstantiateXML(teamXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* playerXml = cache->GetResource<XMLFile>("Prefabs/PlayersLogic/Player.xml");
	if (playerXml)
		playerNode_ = scene_->InstantiateXML(playerXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	// LEGACY
	legacyEquipment_.Resize(LEquipment::EquipmentType::ET_Count);

	// engines
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_Engine, "LegacyPrefabs/Engines/sun.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_Engine, "LegacyPrefabs/Engines/chemical.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_Engine, "LegacyPrefabs/Engines/ion.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_Engine, "LegacyPrefabs/Engines/plasma.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_Engine, "LegacyPrefabs/Engines/nuclear.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_Engine, "LegacyPrefabs/Engines/photon.xml");

	// hyper engines
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_HyperDrive, "LegacyPrefabs/HyperEngines/sun.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_HyperDrive, "LegacyPrefabs/HyperEngines/chemical.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_HyperDrive, "LegacyPrefabs/HyperEngines/ion.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_HyperDrive, "LegacyPrefabs/HyperEngines/plasma.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_HyperDrive, "LegacyPrefabs/HyperEngines/nuclear.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_HyperDrive, "LegacyPrefabs/HyperEngines/photon.xml");

	// weapon
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_Weapon, "LegacyPrefabs/Weapon/MachineGun/machineGun.xml");

	// energy field generators
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_FieldGenerator, "LegacyPrefabs/EnergyFieldGenerators/sun.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_FieldGenerator, "LegacyPrefabs/EnergyFieldGenerators/chemical.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_FieldGenerator, "LegacyPrefabs/EnergyFieldGenerators/ion.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_FieldGenerator, "LegacyPrefabs/EnergyFieldGenerators/plasma.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_FieldGenerator, "LegacyPrefabs/EnergyFieldGenerators/nuclear.xml");
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_FieldGenerator, "LegacyPrefabs/EnergyFieldGenerators/photon.xml");

	// crystal detector
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_CrystalDetector, "LegacyPrefabs/Equipment/CrystalDetector.xml");

	// repair drone
	LoadLegacyEquipmentFromXML(LEquipment::EquipmentType::ET_RepairDrone, "LegacyPrefabs/Equipment/RepairDrone.xml");

	// buildings
	buildings_.Resize(LBuilding::TargetType::TT_Count);

	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/EngineFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/EquipmentFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/MetalsExtractionFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/OrbitalFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/ProbesFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/Radar.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/RepairFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/ScrapMetalFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/shieldFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/ShipFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/WeaponFactory.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Planet, "LegacyPrefabs/Buildings/DefenderPlatformPlanet.xml");

	LoadBuildingFromXML(LBuilding::TargetType::TT_Star, "LegacyPrefabs/Buildings/DefenderPlatformStar.xml");
	LoadBuildingFromXML(LBuilding::TargetType::TT_Star, "LegacyPrefabs/Buildings/SolarStation.xml");

	LoadBuildingFromXML(LBuilding::TargetType::TT_NeutronStar, "LegacyPrefabs/Buildings/ScienceLaboratory.xml");

	// ships
	XMLFile* probeXml = cache->GetResource<XMLFile>("LegacyPrefabs/Ships/ResearchProbe.xml");
	if (probeXml)
		probeNode_ = scene_->InstantiateXML(probeXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* legacyBaseShipXml = cache->GetResource<XMLFile>("LegacyPrefabs/Ships/BaseShip.xml");
	if (baseShipXml)
		baseShipNode_ = scene_->InstantiateXML(baseShipXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	// space objects
	XMLFile* legacyStarXml = cache->GetResource<XMLFile>("LegacyPrefabs/SpaceObjects/Star.xml");
	if (starXml)
		starNode_ = scene_->InstantiateXML(starXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* legacyPlanetXml = cache->GetResource<XMLFile>("LegacyPrefabs/SpaceObjects/Planet.xml");
	if (planetXml)
		planetNode_ = scene_->InstantiateXML(planetXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* legacyAsteroidXml = cache->GetResource<XMLFile>("LegacyPrefabs/SpaceObjects/Asteroid.xml");
	if (asteroidXml)
		asteroidNode_ = scene_->InstantiateXML(asteroidXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* legacyDebrisXml = cache->GetResource<XMLFile>("LegacyPrefabs/SpaceObjects/Debris.xml");
	if (debrisXml)
		debrisNode_ = scene_->InstantiateXML(debrisXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	// player logic
	XMLFile* legacyTeamXml = cache->GetResource<XMLFile>("LegacyPrefabs/PlayersLogic/Team.xml");
	if (teamXml)
		teamNode_ = scene_->InstantiateXML(teamXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);

	XMLFile* legacyPlayerXml = cache->GetResource<XMLFile>("LegacyPrefabs/PlayersLogic/Player.xml");
	if (playerXml)
		playerNode_ = scene_->InstantiateXML(playerXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
}

const Urho3D::Node* PrefabsStorage::GetEquipment(TEquipment::EquipmentType equipmentType, const String& equipmentName) const
{
	if (!equipment_[equipmentType].Contains(equipmentName))
		return nullptr;

	return equipment_[equipmentType].Find(equipmentName)->second_;
}

Urho3D::Node* PrefabsStorage::CreateEquipment(Node* playerNode, TEquipment::EquipmentType equipmentType, const String& equipmentName) const
{
	if (!playerNode || !equipment_[equipmentType].Contains(equipmentName))
		return nullptr;

	auto it = equipment_[equipmentType].Find(equipmentName);
	if (!it->second_)
		return nullptr;

	TEquipment* equipment = it->second_->GetComponent<TEquipment>();
	if (!equipment)
		return nullptr;

	//if (equipment->technology_ == Research::ResearchType::Base)
		return it->second_->Clone();

	//return nullptr;
}

bool PrefabsStorage::IsEquipmentAvailable(Node* playerNode, TEquipment::EquipmentType equipmentType, const String& equipmentName) const
{
	if (!equipment_[equipmentType].Contains(equipmentName))
		return false;

	auto it = equipment_[equipmentType].Find(equipmentName);
	if (!it->second_)
		return false;

	TEquipment* equipment = it->second_->GetComponent<TEquipment>();
	if (!equipment)
		return false;

	//if (equipment->technology_ == Research::ResearchType::Base)
		return true;

	//return true;
}

const HashMap<Urho3D::String, Urho3D::Node*> PrefabsStorage::GetAvailabeEquipment(TEquipment::EquipmentType equipmentType, Urho3D::Node* playerNode) const
{
	HashMap<Urho3D::String, Urho3D::Node*> result;
	for (auto& item : equipment_[equipmentType])
	{
		if (IsEquipmentAvailable(playerNode, equipmentType, item.first_))
		{
			result.Insert(Pair<Urho3D::String, Urho3D::Node*>(item.first_, item.second_));
		}
	}

	return result;
}

const HashMap<Urho3D::String, Urho3D::Node*> PrefabsStorage::GetAllEquipment(TEquipment::EquipmentType equipmentType) const
{
	HashMap<Urho3D::String, Urho3D::Node*> result;
	for (auto& item : equipment_[equipmentType])
	{
		result.Insert(Pair<Urho3D::String, Urho3D::Node*>(item.first_, item.second_));
	}

	return result;
}

Urho3D::Node * PrefabsStorage::CreateProbe(Node * playerNode) const
{
	if (!playerNode || !probeNode_)
		return nullptr;

	ResearchProbe* probe = probeNode_->GetComponent<ResearchProbe>();
	LResearchTree* tree = playerNode->GetParentComponent<LResearchTree>(true);
	if (!probe || !tree)
		return nullptr;

	// TODO: ?
	//Research::ResearchType needTechnology = probe->technology_;
	//if (tree->GetCompletedResearches().Contains(needTechnology))
		return probeNode_->Clone();

	return nullptr;
}

Urho3D::Node* PrefabsStorage::GetMob() const
{
	if (!mobNode_)
		return nullptr;

	return mobNode_->Clone();
}

Urho3D::Node * PrefabsStorage::GetBaseShip() const
{
	if (baseShipNode_)
		return baseShipNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetStar() const
{
	if (starNode_)
		return starNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetPlanet() const
{
	if (planetNode_)
		return planetNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetAsteroid() const
{
	if (asteroidNode_)
		return asteroidNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetDebris() const
{
	if (debrisNode_)
		return debrisNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetTeam() const
{
	if (teamNode_)
		return teamNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetPlayer() const
{
	if (playerNode_)
		return playerNode_->Clone();

	return nullptr;
}

bool PrefabsStorage::IsBuildingAvailable(Node * playerNode, LBuilding::TargetType targetType, const String& buildingName) const
{
	if (!buildings_[targetType].Contains(buildingName) || !playerNode || targetType == LBuilding::TargetType::TT_Count)
		return false;

	auto it = buildings_[targetType].Find(buildingName);
	if (!it->second_)
		return false;

	LBuilding* building = it->second_->GetComponent<LBuilding>();
	if (!building)
		return false;

	if (building->technology_ == Research::ResearchType::Base)
		return true;

	LResearchTree* tree = playerNode->GetParentComponent<LResearchTree>(true);
	if (!tree || !tree->GetCompletedResearches().Contains(building->technology_))
		return false;

	return true;
}

const HashSet<Urho3D::String> PrefabsStorage::GetAvailableBuildings(Node * playerNode, LBuilding::TargetType targetType) const
{
	HashSet<Urho3D::String> result;
	for (auto& item : buildings_[targetType])
	{
		if (IsBuildingAvailable(playerNode, targetType, item.first_))
		{
			result.Insert(item.first_);
		}
	}

	return result;
}

Urho3D::Node * PrefabsStorage::GetLegacyBaseShip() const
{
	if (legacyBaseShipNode_)
		return legacyBaseShipNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetLegacyStar() const
{
	if (legacyStarNode_)
		return legacyStarNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetLegacyPlanet() const
{
	if (legacyPlanetNode_)
		return legacyPlanetNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetLegacyAsteroid() const
{
	if (legacyAsteroidNode_)
		return legacyAsteroidNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetLegacyDebris() const
{
	if (legacyDebrisNode_)
		return legacyDebrisNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetLegacyTeam() const
{
	if (legacyTeamNode_)
		return legacyTeamNode_->Clone();

	return nullptr;
}

Urho3D::Node * PrefabsStorage::GetLegacyPlayer() const
{
	if (legacyPlayerNode_)
		return legacyPlayerNode_->Clone();

	return nullptr;
}

const Urho3D::Node* PrefabsStorage::GetBuilding(LBuilding::TargetType targetType, const String& buildingName) const
{
	if (!buildings_[targetType].Contains(buildingName))
		return nullptr;

	return buildings_[targetType].Find(buildingName)->second_;
}

Urho3D::Node * PrefabsStorage::CreateBuilding(Node * playerNode, LBuilding::TargetType targetType, const String & buildingName) const
{
	if (!playerNode || targetType == LBuilding::TargetType::TT_Count|| !buildings_[targetType].Contains(buildingName))
		return nullptr;

	auto it = buildings_[targetType].Find(buildingName);
	if (!it->second_)
		return nullptr;

	LBuilding* building = it->second_->GetComponent<LBuilding>();
	if (!building)
		return nullptr;

	if (building->technology_ == Research::ResearchType::Base)
		return it->second_->Clone();

	LResearchTree* tree = playerNode->GetParentComponent<LResearchTree>(true);
	if (!tree)
		return nullptr;

	if (!tree->GetCompletedResearches().Contains(building->technology_))
		return nullptr;

	return it->second_->Clone();
}

Pair<TGameResource::ResourceType, F32> PrefabsStorage::GetEquipmentCreateCost(TEquipment::EquipmentType equipmentType, const String& equipmentTypeName) const
{
	if (!equipment_[equipmentType].Contains(equipmentTypeName))
		return Pair<TGameResource::ResourceType, F32>(TGameResource::ResourceType::Count, 0);

	TCost* cost = equipment_[equipmentType].Find(equipmentTypeName)->second_->GetComponent<TCost>();
	if (!cost)
		return Pair<TGameResource::ResourceType, F32>(TGameResource::ResourceType::Count, 0);

	return Pair<TGameResource::ResourceType, F32>(cost->GetCostType(), cost->GetCreateCost());
}

Pair<LGameResource::ResourceType, F32> PrefabsStorage::GetLegacyEquipmentCreateCost(LEquipment::EquipmentType equipmentType, const String & equipmentTypeName) const
{
	if (!legacyEquipment_[equipmentType].Contains(equipmentTypeName))
		return Pair<LGameResource::ResourceType, F32>(LGameResource::ResourceType::Count, 0);

	LCost* cost = legacyEquipment_[equipmentType].Find(equipmentTypeName)->second_->GetComponent<LCost>();
	if (!cost)
		return Pair<LGameResource::ResourceType, F32>(LGameResource::ResourceType::Count, 0);

	return Pair<LGameResource::ResourceType, F32>(cost->GetCostType(), cost->GetCreateCost());
}

Pair<LGameResource::ResourceType, F32> PrefabsStorage::GetBuildingCreateCost(LBuilding::TargetType targetType, const String & buildingTypeName) const
{
	if (!buildings_[targetType].Contains(buildingTypeName))
		return Pair<LGameResource::ResourceType, F32>(LGameResource::ResourceType::Count, 0);

	LCost* cost = buildings_[targetType].Find(buildingTypeName)->second_->GetComponent<LCost>();
	if (!cost)
		return Pair<LGameResource::ResourceType, F32>(LGameResource::ResourceType::Count, 0);

	return Pair<LGameResource::ResourceType, F32>(cost->GetCostType(), cost->GetCreateCost());
}

Pair<LGameResource::ResourceType, F32> PrefabsStorage::GetProbeCreateCost() const
{
	if (!probeNode_)
		return Pair<LGameResource::ResourceType, F32>(LGameResource::ResourceType::Count, 0);

	LCost* cost = probeNode_->GetComponent<LCost>();
	if (!cost)
		return Pair<LGameResource::ResourceType, F32>(LGameResource::ResourceType::Count, 0);

	return Pair<LGameResource::ResourceType, F32>(cost->GetCostType(), cost->GetCreateCost());
}

Pair<TGameResource::ResourceType, F32> PrefabsStorage::GetMobCreateCost() const
{
	if (!mobNode_)
		return Pair<TGameResource::ResourceType, F32>(TGameResource::ResourceType::Count, 0);

	TCost* cost = mobNode_->GetComponent<TCost>();
	if (!cost)
		return Pair<TGameResource::ResourceType, F32>(TGameResource::ResourceType::Count, 0);

	return Pair<TGameResource::ResourceType, F32>(cost->GetCostType(), cost->GetCreateCost());
}

const Urho3D::Node * PrefabsStorage::GetLegacyEquipment(LEquipment::EquipmentType equipmentType, const String & equipmentName) const
{
	if (!legacyEquipment_[equipmentType].Contains(equipmentName))
		return nullptr;

	return legacyEquipment_[equipmentType].Find(equipmentName)->second_;
}

Urho3D::Node * PrefabsStorage::CreateLegacyEquipment(Node * playerNode, LEquipment::EquipmentType equipmentType, const String & equipmentName) const
{
	if (!playerNode || !legacyEquipment_[equipmentType].Contains(equipmentName))
		return nullptr;

	auto it = legacyEquipment_[equipmentType].Find(equipmentName);
	if (!it->second_)
		return nullptr;

	LEquipment* equipment = it->second_->GetComponent<LEquipment>();
	if (!equipment)
		return nullptr;

	if (equipment->technology_ == Research::ResearchType::Base)
		return it->second_->Clone();

	LResearchTree* tree = playerNode->GetParentComponent<LResearchTree>(true);
	if (!tree)
		return nullptr;

	if (!tree->GetCompletedResearches().Contains(equipment->technology_))
		return nullptr;

	return it->second_->Clone();
}

bool PrefabsStorage::IsLegacyEquipmentAvailable(Node * playerNode, LEquipment::EquipmentType equipmentType, const String & equipmentName) const
{
	if (!legacyEquipment_[equipmentType].Contains(equipmentName))
		return false;

	auto it = legacyEquipment_[equipmentType].Find(equipmentName);
	if (!it->second_)
		return false;

	LEquipment* equipment = it->second_->GetComponent<LEquipment>();
	if (!equipment)
		return false;

	if (equipment->technology_ == Research::ResearchType::Base)
		return true;

	if (!playerNode)
		return false;

	LResearchTree* tree = playerNode->GetParentComponent<LResearchTree>(true);
	if (!tree || !tree->GetCompletedResearches().Contains(equipment->technology_))
		return false;

	return true;
}

const HashMap<Urho3D::String, Urho3D::Node*> PrefabsStorage::GetAvailabeLegacyEquipment(LEquipment::EquipmentType equipmentType, Urho3D::Node * playerNode) const
{
	HashMap<Urho3D::String, Urho3D::Node*> result;
	for (auto& item : legacyEquipment_[equipmentType])
	{
		if (IsLegacyEquipmentAvailable(playerNode, equipmentType, item.first_))
		{
			result.Insert(Pair<Urho3D::String, Urho3D::Node*>(item.first_, item.second_));
		}
	}

	return result;
}

void PrefabsStorage::LoadEquipmentFromXML(TEquipment::EquipmentType equipmentType, const String& xmlName)
{
	static ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* xmlFile = cache->GetResource<XMLFile>(xmlName);
	if (xmlFile)
	{
		Node* prefabNode = scene_->InstantiateXML(xmlFile->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
		if (prefabNode)
		{
			equipment_[equipmentType][prefabNode->GetName()] = prefabNode;
		}
	}
}

void PrefabsStorage::LoadLegacyEquipmentFromXML(LEquipment::EquipmentType equipmentType, const String & xmlName)
{
	static ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* xmlFile = cache->GetResource<XMLFile>(xmlName);
	if (xmlFile)
	{
		Node* prefabNode = scene_->InstantiateXML(xmlFile->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
		if (prefabNode)
		{
			legacyEquipment_[equipmentType][prefabNode->GetName()] = prefabNode;
		}
	}
}

void PrefabsStorage::LoadBuildingFromXML(LBuilding::TargetType targetType, const String& xmlName)
{
	static ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* xmlFile = cache->GetResource<XMLFile>(xmlName);
	if (xmlFile)
	{
		Node* prefabNode = scene_->InstantiateXML(xmlFile->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
		if (prefabNode)
		{
			buildings_[targetType][prefabNode->GetName()] = prefabNode;
		}
	}
}
