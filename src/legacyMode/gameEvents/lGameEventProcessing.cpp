#include <Urho3D/Network/Network.h>

#include "legacyMode/buildings/shipFactory.h"
#include "legacyMode/equipment/hyperDrive.h"
#include "legacyMode/equipment/crystalDetector.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/playerLogic/lResearchTree.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/managers/lBuildingsManager.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/playerLogic/lTeam.h"
#include "prefabs/prefabsStorage.h"
#include "legacyMode/spaceship/lDurability.h"
#include "legacyMode/buildings/lBuilding.h"
#include "tacticalMode/gameEvents/tGameEvents.h"

#include "legacyMode/gameEvents/lGameEventProcessing.h"

using namespace Urho3D;

LGameEventProcessing::LGameEventProcessing(Urho3D::Context* context) :
	Object(context)
{
}

LGameEventProcessing::~LGameEventProcessing()
{
}

void LGameEventProcessing::SubscribeToNetworkEvents()
{
	// OBJECT
	SubscribeToEvent(RE_OBJECT_BUILD, URHO3D_HANDLER(LGameEventProcessing, HandleObjectBuild));
	SubscribeToEvent(RE_GET_BUILDING_RESOURCE, URHO3D_HANDLER(LGameEventProcessing, HandleTakeResource));
	SubscribeToEvent(RE_IMPROVE_BUILDING, URHO3D_HANDLER(LGameEventProcessing, HandleImproveBuilding));

	// PLANET
	SubscribeToEvent(RE_CREATE_PROBE, URHO3D_HANDLER(LGameEventProcessing, HandleCreateProbe));
	SubscribeToEvent(RE_SHIP_REPAIR, URHO3D_HANDLER(LGameEventProcessing, HandleShipRepair));
	SubscribeToEvent(RE_CREATE_EQUIPMENT, URHO3D_HANDLER(LGameEventProcessing, HandleCreateEquipment));
	SubscribeToEvent(RE_MOBS_PRODUCTION, URHO3D_HANDLER(LGameEventProcessing, HandleStartMobsProduction));
	SubscribeToEvent(RE_PUT_ON_SCRAP, URHO3D_HANDLER(LGameEventProcessing, HandleScrap));

	// MAP
	SubscribeToEvent(RE_HYPER_DRIVE, URHO3D_HANDLER(LGameEventProcessing, HandleHyperDrive));

	// CRYSTAL
	SubscribeToEvent(RE_DETECTOR_USE, URHO3D_HANDLER(LGameEventProcessing, HandleCrystalDetectorUse));

	// Ship info
	SubscribeToEvent(RE_EQUIPMENT_MOVE, URHO3D_HANDLER(LGameEventProcessing, HandleEquipmentMove));

	// RESEARCH
	SubscribeToEvent(RE_RESEARCH_SELECTION, URHO3D_HANDLER(LGameEventProcessing, HandleSelectResearch));
}

void LGameEventProcessing::SetScene(Scene * scene)
{
	scene_ = scene;
}

bool LGameEventProcessing::IsServerConnection() const
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return true;

	return false;
}

void LGameEventProcessing::HandleObjectBuild(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 playerID = eventData[ObjectBuildEvent::PLAYER_ID].GetUInt();
	U32 objectID = eventData[ObjectBuildEvent::OBJECT_ID].GetUInt();
	String buildingName = eventData[ObjectBuildEvent::BUILDING_NAME].GetString();
	LBuilding::TargetType targetType = static_cast<LBuilding::TargetType>(eventData[ObjectBuildEvent::TARGET_TYPE].GetUInt());

	LResourceManager* rManager = LGeneralMethods::GetObjectManager<LResourceManager>(scene_, playerID);
	LBuildingsManager* bManager = LGeneralMethods::GetObjectManager<LBuildingsManager>(scene_, objectID);

	auto cost = GetSubsystem<PrefabsStorage>()->GetBuildingCreateCost(targetType, buildingName);
	if (bManager && rManager && rManager->ChangeResourceValue(cost.first_, cost.second_))
		bManager->AddBuilding(buildingName, playerID);
}

void LGameEventProcessing::HandleTakeResource(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 playerID = eventData[GetBuildingResourceEnergyEvent::PLAYER_ID].GetUInt();
	U32 planetID = eventData[GetBuildingResourceEnergyEvent::OBJECT_ID].GetUInt();
	String buildingName = eventData[GetBuildingResourceEnergyEvent::BUILDING_NAME].GetString();
	LGameResource::ResourceType resourceType = static_cast<LGameResource::ResourceType>(eventData[GetBuildingResourceEnergyEvent::RESOURCE_TYPE].GetUInt());

	LResourceManager* bRManager = LGeneralMethods::GetPlanetBuildingResourceManager(scene_, planetID, buildingName);
	LResourceManager* shRManager = LGeneralMethods::GetObjectManager<LResourceManager>(scene_, playerID);

	if (bRManager && shRManager)
	{
		LGameResource::ResourceType type = static_cast<LGameResource::ResourceType>(resourceType);
		shRManager->ChangeResourceValue(type, bRManager->GetResourceValue(type));
		shRManager->SetResourceValue(type, 0.f);
	}
}

void LGameEventProcessing::HandleImproveBuilding(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 objectID = eventData[ImproveBuildingEvent::OBJECT_ID].GetUInt();
	U32 playerID = eventData[ImproveBuildingEvent::PLAYER_ID].GetUInt();
	String buildingName = eventData[ImproveBuildingEvent::BUILDING_NAME].GetString();
	LBuilding::TargetType targetType = static_cast<LBuilding::TargetType>(eventData[ImproveBuildingEvent::TARGET_TYPE].GetUInt());

	LResourceManager* rManager = LGeneralMethods::GetObjectManager<LResourceManager>(scene_, playerID);
	LBuildingsManager* bManager = LGeneralMethods::GetObjectManager<LBuildingsManager>(scene_, objectID);

	auto cost = GetSubsystem<PrefabsStorage>()->GetBuildingCreateCost(targetType, buildingName);
	if (bManager && rManager && rManager->ChangeResourceValue(cost.first_, cost.second_))
		bManager->ImproveBuilding(buildingName);
}

void LGameEventProcessing::HandleCreateProbe(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 playerID = eventData[CreateProbeEvent::PLAYER_ID].GetUInt();

	LResourceManager* rManager = LGeneralMethods::GetObjectManager<LResourceManager>(scene_, playerID);
	LTeam* team = LGeneralMethods::GetTeam(scene_, playerID);

	auto cost = GetSubsystem<PrefabsStorage>()->GetProbeCreateCost();
	if (team && rManager && rManager->ChangeResourceValue(cost.first_, cost.second_))
		team->AddProbe();
}

void LGameEventProcessing::HandleShipRepair(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 playerID = eventData[RepairShipEvent::PLAYER_ID].GetUInt();

	LResourceManager* rManager = LGeneralMethods::GetObjectManager<LResourceManager>(scene_, playerID);
	LEquipmentManager* eManager = LGeneralMethods::GetObjectManager<LEquipmentManager>(scene_, playerID);

	LDurability* shipDurability = eManager->GetNode()->GetComponent<LDurability>();
	if (shipDurability && rManager && rManager->ChangeResourceValue(LGameResource::Metals, shipDurability->GetDurability() - shipDurability->GetMaxDurability()))
		shipDurability->SetDurability(shipDurability->GetMaxDurability());
}

void LGameEventProcessing::HandleCreateEquipment(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 playerId = eventData[CreateEquipmentEvent::PLAYER_ID].GetUInt();
	LEquipment::EquipmentType equipmentType = static_cast<LEquipment::EquipmentType>(eventData[CreateEquipmentEvent::EQUIPMENT_TYPE].GetUInt());
	String equipmentName = eventData[CreateEquipmentEvent::EQUIPMENT_NAME].GetString();
	
	PrefabsStorage* prefabs = GetSubsystem<PrefabsStorage>();
	auto cost = prefabs->GetLegacyEquipmentCreateCost(equipmentType, equipmentName);

	LResourceManager* rManager = LGeneralMethods::GetObjectManager<LResourceManager>(scene_, playerId);
	LEquipmentManager* eManager = LGeneralMethods::GetObjectManager<LEquipmentManager>(scene_, playerId);

	if (eManager && rManager && rManager->ChangeResourceValue(cost.first_, cost.second_))
	{
		Node* equipmentNode = prefabs->CreateLegacyEquipment(eManager->GetNode(), equipmentType, equipmentName);
		eManager->AddElementToCargo(equipmentNode);
	}
}

void LGameEventProcessing::HandleStartMobsProduction(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 planetID = eventData[ProductionMobsEvent::PLANET_ID].GetUInt();
	U32 mobNumber = eventData[ProductionMobsEvent::MOB_NUMBER].GetUInt();

	LBuildingsManager* manager = LGeneralMethods::GetObjectManager<LBuildingsManager>(scene_, planetID);
	if (!manager)
		return;

	// ship factory
	LBuilding* bldg = manager->GetBuilding("shipFactory");
	ShipFactory* shipFactory = bldg->GetNode()->GetComponent<ShipFactory>();
	if (shipFactory)
		shipFactory->AddProduceShip(mobNumber);
}

void LGameEventProcessing::HandleScrap(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 playerID = eventData[PutOnScrapEvent::PLAYER_ID].GetUInt();
	U32 elementID = eventData[PutOnScrapEvent::CARGO_ELEMENT_ID].GetUInt();

	LResourceManager* rManager = LGeneralMethods::GetObjectManager<LResourceManager>(scene_, playerID);
	LEquipmentManager* eManager = LGeneralMethods::GetObjectManager<LEquipmentManager>(scene_, playerID);
	if (!rManager || !eManager)
		return;

	// remove from cargo
	eManager->RemoveElementFromCargo(elementID);

	// add metals
	rManager->ChangeResourceValue(LGameResource::ResourceType::Metals, Random(0.f, 50.f));
}

void LGameEventProcessing::HandleHyperDrive(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 playerID = eventData[StarSystemDriveEvent::PLAYER_ID].GetUInt();
	U32 starSystemID = eventData[StarSystemDriveEvent::STAR_SYSTEM_ID].GetUInt();

	// player
	LEquipmentManager* manager = LGeneralMethods::GetObjectManager<LEquipmentManager>(scene_, playerID);
	if (!manager)
		return;

	// hyper drive
	HyperDrive* hyperDrive = manager->GetEquipmentItem<HyperDrive>();
	Node* starSystemNode = LGeneralMethods::GetObjectNode(scene_, starSystemID);
	if (hyperDrive && starSystemNode)
	{
		hyperDrive->SetStarSystem(starSystemNode->GetID());
		hyperDrive->TravelToPosition(starSystemNode->GetWorldPosition2D());
	}
}

void LGameEventProcessing::HandleCrystalDetectorUse(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 playerID = eventData[CrystalDetectorUseEvent::PLAYER_ID].GetUInt();

	LEquipmentManager* eManager = LGeneralMethods::GetObjectManager<LEquipmentManager>(scene_, playerID);
	if (!eManager)
		return;

	bool isMainPlayer = LGeneralMethods::IsMainPlayer(eManager->GetNode());

	// crystal detector
	CrystalDetector* crystalDetector = eManager->GetEquipmentItem<CrystalDetector>();
	if (crystalDetector && crystalDetector->Use() && isMainPlayer)
	{
		String infoString = "New crystal !";
		SendEvent(RE_INGAME_INFO_STRING, GameInfo::INFO_STRING, infoString);
	}
}

void LGameEventProcessing::HandleEquipmentMove(StringHash eventType, VariantMap & eventData)
{
	U32 nodeID = eventData[EquipmentMoveEvent::NODE_ID].GetUInt();
	U32 newParentID = eventData[EquipmentMoveEvent::NEW_PARENT_ID].GetUInt();

	Node* node = scene_->GetNode(nodeID);
	Node* newParent = scene_->GetNode(newParentID);

	if (node && newParent)
		newParent->AddChild(node);
}

void LGameEventProcessing::HandleSelectResearch(StringHash eventType, VariantMap & eventData)
{
	if (IsServerConnection() || !scene_)
		return;

	U32 teamNumber = eventData[ResearchSelectionEvent::TEAM_NUMBER].GetUInt();
	U32 ResearchType = eventData[ResearchSelectionEvent::RESEARCH_TYPE].GetUInt();

	Node* teamNode = LGeneralMethods::GetTeamNode(scene_, teamNumber);
	if (!teamNode)
		return;

	// research
	LResearchTree* research = teamNode->GetComponent<LResearchTree>();
	LResourceManager* rManager = teamNode->GetComponent<LResourceManager>();
	if (research && rManager && rManager->ChangeResourceValue(LGameResource::ResourceType::Knowledge, -research->GetResearchCost(ResearchType)))
		research->StartResearch(ResearchType);
}
