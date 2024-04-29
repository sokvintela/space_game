#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>

#include "legacyMode/playerLogic/owner.h"
#include "prefabs/prefabsStorage.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/playerLogic/lTeam.h"
#include "legacyMode/buildings/lBuilding.h"

#include "legacyMode/managers/lBuildingsManager.h"

String buildings = "buildings";

void LBuildingsManager::RegisterObject(Context* context)
{
	context->RegisterFactory<LBuildingsManager>();
}

LBuildingsManager::LBuildingsManager(Context * context) :
	LogicComponent(context)
{
}

void LBuildingsManager::Create() const
{
	Node* buildingsNode = node_->CreateChild(buildings);
}

HashSet<String> LBuildingsManager::GetProposalBuildings(Node* playerNode) const
{
	HashSet<String> result = GetAvailableBuildings(playerNode);

	PODVector<Node*> buildings = GetCompletedBuildings();
	for (auto building : buildings)
		result.Erase(building->GetName());

	return result;
}

PODVector<Node*> LBuildingsManager::GetCompletedBuildings() const
{
	return node_->GetChildrenWithComponent(LBuilding::GetTypeStatic(), true);
}

HashSet<String> LBuildingsManager::GetAvailableBuildings(Node* playerNode) const
{
	return GetSubsystem<PrefabsStorage>()->GetAvailableBuildings(playerNode, LGeneralMethods::GetObjectTargetType(node_));
}

LBuilding* LBuildingsManager::GetBuilding(String buildingName) const
{
	Node* buildingNode = GetBuildingNode(buildingName);
	if (!buildingNode)
		return nullptr;

	return buildingNode->GetComponent<LBuilding>();
}

void LBuildingsManager::AddBuilding(String buildingName, U32 ownerID) const
{
	bool isFastGame = false;
	LBuilding::TargetType tType = LGeneralMethods::GetObjectTargetType(node_);

	Node* playerNode = LGeneralMethods::GetObjectNode(node_, ownerID);
	if (!playerNode)
		return;

	Node* buildingNode = GetSubsystem<PrefabsStorage>()->CreateBuilding(playerNode, tType, buildingName);
	LBuilding* building = buildingNode->GetComponent<LBuilding>();
	building->Create(buildingName, tType);

	AddOrChangeObjectOwner(ownerID);

	LTeam* team = LGeneralMethods::GetTeam(node_, ownerID);
	if (team)
		AddOrChangeObjectTeam(team->GetTeamNumber());

	node_->AddChild(buildingNode);
}

void LBuildingsManager::ImproveBuilding(String buildingName)
{
	Node* buildingNode = nullptr;
	for (auto child : node_->GetChildren())
	{
		LBuilding* building = child->GetComponent<LBuilding>();
		if (building->GetNode()->GetName() == buildingName)
		{
			buildingNode = child;
			break;
		}
	}

	if (buildingName == "radar")
	{
		LResourceManager* rManager = buildingNode->GetComponent<LResourceManager>();
		rManager->AddResourceDevice(LGameResource::ResourceDevice::Transmitter, LGameResource::ResourceType::Energy);
	}
}

void LBuildingsManager::AddOrChangeObjectOwner(S32 ownerID) const
{
	Owner* owner = node_->GetOrCreateComponent<Owner>();
	owner->SetOwnerID(ownerID);
}

Node* LBuildingsManager::GetBuildingNode(String buildingName) const
{
	for (auto child : node_->GetChildren())
		if (child->GetName() == buildingName)
			return child;

	return nullptr;
}

void LBuildingsManager::AddOrChangeObjectTeam(S32 teamNumber) const
{
	Owner* owner = node_->GetOrCreateComponent<Owner>();
	owner->SetTeamNumber(teamNumber);
}
