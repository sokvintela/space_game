#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>

#include "legacyMode/playerLogic/lTeam.h"
#include "legacyMode/galaxy/lStarSystem.h"
#include "legacyMode/managers/lBuildingsManager.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "equipment/shipEngine.h"
#include "legacyMode/managers/lDataManager.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/utility/lSharedData.h"
#include "legacyMode/playerLogic/owner.h"
#include "legacyMode/spaceship/lSpaceship.h"
#include "legacyMode/buildings/lBuilding.h"

#include "legacyMode/utility/lGeneralMethods.h"

using namespace Urho3D;

static const F32 mathPi = static_cast<F32>(M_PI);

Urho3D::Vector2 LGeneralMethods::GeneratePositionInCircle(F32 circleRadius)
{
	F32 a = Random();
	F32 b = Random();

	if (b < a)
	{
		b += a;
		a = b - a;
		b -= a;
	}

	return Vector2(b * circleRadius * cosf(2.f * mathPi * a / b), b * circleRadius * sinf(2.f * mathPi * a / b));
}

LStarSystem * LGeneralMethods::GetStarSystem(Urho3D::Node * someNode, S32 systemID)
{
	if (!someNode || systemID == -1)
		return nullptr;

	Urho3D::Node* systemNode = GetObjectNode(someNode, systemID);
	if (!systemNode)
		return nullptr;

	return systemNode->GetComponent<LStarSystem>();
}

bool LGeneralMethods::IsMainPlayer(Urho3D::Node * playerNode)
{
	LSharedData* sharedData = playerNode->GetSubsystem<LSharedData>();
	LDataManager* dManager = playerNode->GetComponent<LDataManager>();
	if (dManager && dManager->GetClientID() == sharedData->clientId_)
		return true;

	return false;
}

Urho3D::Node* LGeneralMethods::GetMyControlNode(Urho3D::Node* someNode)
{
	LSharedData* sharedData = someNode->GetSubsystem<LSharedData>();
	return GetClientNode(someNode, sharedData->clientId_, sharedData->myTeam_);
}

Urho3D::Node* LGeneralMethods::GetClientNode(Urho3D::Node * someNode, U32 clientId, U32 teamNumber)
{
	if (!someNode)
		return nullptr;

	Urho3D::Scene* scene = someNode->GetScene();
	if (!scene)
		return nullptr;

	Galaxy* galaxy = scene->GetComponent<Galaxy>(true);
	Urho3D::Node* playersNode = galaxy->GetPlayersNode(teamNumber);

	if (!playersNode)
		return nullptr;

	return playersNode->GetChild(String(clientId));
}

LTeam * LGeneralMethods::GetTeam(Urho3D::Node * someNode, U32 teamNumber)
{
	Node* teamNode = GetTeamNode(someNode, teamNumber);
	if (!teamNode)
		return nullptr;

	return teamNode->GetComponent<LTeam>();
}

Urho3D::Node * LGeneralMethods::GetTeamNode(Urho3D::Node * someNode, U32 teamNumber)
{
	Galaxy* galaxy = someNode->GetScene()->GetComponent<Galaxy>(true);
	Urho3D::Node* playersNode = galaxy->GetPlayersNode(teamNumber);
	if (!playersNode)
		return nullptr;

	return playersNode->GetParent();
}

U32 LGeneralMethods::GetTeamNumber(Urho3D::Node * myNode)
{
	LTeam* team = myNode->GetParentComponent<LTeam>(true);
	if (team)
		return team->GetTeamNumber();

	return LTeam::NoTeamId;
}

U32 LGeneralMethods::GetTeamNumber(Urho3D::Node * someNode, S32 objectID)
{
	return GetTeamNumber(GetObjectNode(someNode, objectID));
}

Urho3D::Node * LGeneralMethods::GetMyTeamNode(Urho3D::Node * someNode)
{
	LSharedData* sharedData = someNode->GetSubsystem<LSharedData>();

	return GetTeamNode(someNode, sharedData->myTeam_);
}

Urho3D::Node * LGeneralMethods::GetObjectNode(Urho3D::Node * someNode, S32 objectID)
{
	if (!someNode || objectID == -1)
		return nullptr;

	Urho3D::Scene* scene = someNode->GetScene();
	if (!scene)
		return nullptr;

	return scene->GetNode(objectID);
}

PODVector<Urho3D::Node*> LGeneralMethods::GetStarSystemNodes(Urho3D::Node * someNode)
{
	PODVector<Urho3D::Node*> systemNodes;

	if (!someNode)
		return systemNodes;

	Urho3D::Scene* scene = someNode->GetScene();
	if (!scene)
		return systemNodes;

	scene->GetChildrenWithComponent<LStarSystem>(systemNodes, true);
	return systemNodes;
}

Node * LGeneralMethods::GetCurrentStarSystemNode(Urho3D::Node * objectNode)
{
	LStarSystem* system = GetCurrentStarSystem(objectNode);
	if (!system)
		return nullptr;

	return system->GetNode();
}

Node * LGeneralMethods::GetCurrentStarSystemNode(Urho3D::Node * someNode, U32 objectID)
{
	LStarSystem* system = GetCurrentStarSystem(someNode, objectID);
	if (!system)
		return nullptr;

	return system->GetNode();
}

LStarSystem * LGeneralMethods::GetCurrentStarSystem(Urho3D::Node * objectNode)
{
	if (!objectNode)
		return nullptr;

	LDataManager* manager = objectNode->GetComponent<LDataManager>();
	if (!manager)
		return nullptr;

	return GetStarSystem(objectNode, manager->GetCurrentStarSystemID());
}

LStarSystem * LGeneralMethods::GetCurrentStarSystem(Urho3D::Node* someNode, U32 objectID)
{
	return GetCurrentStarSystem(GetObjectNode(someNode, objectID));
}

PODVector<Urho3D::Node*> LGeneralMethods::GetShipNodes(Urho3D::Node * someNode)
{
	PODVector<Urho3D::Node*> shipNodes;

	if (!someNode)
		return shipNodes;

	Urho3D::Scene* scene = someNode->GetScene();
	if (!scene)
		return shipNodes;

	scene->GetChildrenWithComponent<LSpaceShip>(shipNodes, true);
	return shipNodes;
}

Urho3D::Node * LGeneralMethods::GetStarNode(Urho3D::Node * playerNode)
{
	LDataManager* dManager = playerNode->GetComponent<LDataManager>();
	if (!dManager || !dManager->IsInStarSystem())
		return nullptr;

	LStarSystem* system = LGeneralMethods::GetStarSystem(playerNode, dManager->GetCurrentStarSystemID());
	if (!system)
		return nullptr;

	LStar* star = system->GetNode()->GetComponent<LStar>(true);
	if (star)
		return star->GetNode();

	return nullptr;
}

PODVector<Urho3D::Node*> LGeneralMethods::GetPlanetNodes(Urho3D::Node * someNode, U32 systemID, LPlanet::PlanetType type)
{
	return LGeneralMethods::GetPlanetNodes(LGeneralMethods::GetObjectNode(someNode, systemID), type);
}

PODVector<Urho3D::Node*> LGeneralMethods::GetPlanetNodes(Urho3D::Node * systemNode, LPlanet::PlanetType type)
{
	// all planets
	PODVector<Urho3D::Node*> planetNodes;

	if (!systemNode)
		return planetNodes;

	systemNode->GetChildrenWithComponent<LPlanet>(planetNodes, true);

	if (type == LPlanet::PlanetType::PT_Count)
		return planetNodes;

	// solid planets
	PODVector<Urho3D::Node*> solidPlanetNodes;
	for (auto planetNode : planetNodes)
	{
		LPlanet* planet = planetNode->GetComponent<LPlanet>();
		if (planet && planet->GetPlanetType() == LPlanet::PlanetType::PT_Solid)
			solidPlanetNodes.Push(planetNode);
	}

	return solidPlanetNodes;
}

LResourceManager * LGeneralMethods::GetPlanetBuildingResourceManager(Urho3D::Node * planetNode, String buildingName)
{
	if (!planetNode)
		return nullptr;

	LBuildingsManager* bManager = planetNode->GetComponent<LBuildingsManager>();
	if (!bManager)
		return nullptr;

	Node* buildingNode = bManager->GetBuildingNode(buildingName);
	if (!buildingNode)
		return nullptr;

	return buildingNode->GetComponent<LResourceManager>();
}

LResourceManager * LGeneralMethods::GetPlanetBuildingResourceManager(Urho3D::Node * someNode, U32 planetID, String buildingName)
{
	if (!someNode || planetID == -1)
		return nullptr;

	Urho3D::Node* planetNode = GetObjectNode(someNode, planetID);
	if (!planetNode)
		return nullptr;

	return GetPlanetBuildingResourceManager(planetNode, buildingName);
}

LDataManager * LGeneralMethods::GetOwnerDataManager(Urho3D::Node * objectNode)
{
	Urho3D::Node* ownerNode = GetOwnerNode(objectNode);
	if (!ownerNode)
		return nullptr;

	return ownerNode->GetComponent<LDataManager>();
}

LDataManager * LGeneralMethods::GetOwnerDataManager(Urho3D::Node * someNode, U32 objectID)
{
	Urho3D::Node* ownerNode = GetOwnerNode(someNode, objectID);
	if (!ownerNode)
		return nullptr;

	return ownerNode->GetComponent<LDataManager>();
}

LBuilding::TargetType LGeneralMethods::GetObjectTargetType(Urho3D::Node * objectNode)
{
	if (!objectNode)
		return LBuilding::TargetType::TT_Count;

	LPlanet* planet = objectNode->GetComponent<LPlanet>();
	if (planet)
		return LBuilding::TargetType::TT_Planet;

	LStar* star = objectNode->GetComponent<LStar>();
	if (star && star->IsEnergyStar())
		return LBuilding::TargetType::TT_Star;
	else if (star)
		return LBuilding::TargetType::TT_NeutronStar;

	return LBuilding::TargetType::TT_Count;
}

bool LGeneralMethods::MoveObject(Node * objectNode, Vector2 direction, F32 limitDistance)
{
	if (!objectNode)
		return false;

	if (direction.Length() < limitDistance)
		return true;

	LEquipmentManager* manager = objectNode->GetComponent<LEquipmentManager>();
	if (!manager)
		return false;

	ShipEngine* engine = manager->GetEquipmentItem<ShipEngine>();
	RigidBody2D* rigidBody = objectNode->GetComponent<RigidBody2D>();
	if (rigidBody && engine)
	{
		rigidBody->ApplyForceToCenter(direction * engine->GetSpeed(), true);
		return true;
	}

	return false;
}

bool LGeneralMethods::MoveObject(Urho3D::Node * objectNode, Urho3D::Node * targetNode, F32 limitDistance)
{
	if (!objectNode || !targetNode)
		return false;

	Vector2 direction = targetNode->GetWorldPosition2D() - objectNode->GetWorldPosition2D();
	if (direction.Length() < limitDistance)
		return true;

	direction.Normalize();
	return MoveObject(objectNode, direction, limitDistance);
}

bool LGeneralMethods::MoveObject(Urho3D::Node * objectNode, U32 targetID, F32 limitDistance)
{
	if (!objectNode)
		return false;

	Node* targetNode = LGeneralMethods::GetObjectNode(objectNode, targetID);
	if (!targetNode)
		return false;

	if ((targetNode->GetWorldPosition2D() - objectNode->GetWorldPosition2D()).Length() < 1.f)
		return true;

	return MoveObject(objectNode, targetNode, limitDistance);
}

Urho3D::Node* LGeneralMethods::GetClosestPlanetNode(Urho3D::Node * myNode, LPlanet::PlanetType type)
{
	if (!myNode)
		return nullptr;

	F32 minDistance = INFINITY;
	Urho3D::Node* closestNode = nullptr;

	PODVector<Urho3D::Node*> planetNodes = LGeneralMethods::GetPlanetNodes(GetCurrentStarSystemNode(myNode), type);
	for (U32 i = 0; i < planetNodes.Size(); i++)
	{
		F32 distance = (planetNodes[i]->GetWorldPosition2D() - myNode->GetWorldPosition2D()).Length();
		if (distance < minDistance)
		{
			minDistance = distance;
			closestNode = planetNodes[i];
		}
	}

	return closestNode;
}

Urho3D::Vector2 LGeneralMethods::GetClosestPlanetPosition(Urho3D::Node * myNode, LPlanet::PlanetType type)
{
	Node* targetNode = GetClosestPlanetNode(myNode, type);
	if (!targetNode)
		return myNode->GetWorldPosition2D();

	return targetNode->GetWorldPosition2D();
}

Owner * LGeneralMethods::GetOwner(Urho3D::Node* someNode, U32 objectID)
{
	if (!someNode)
		return nullptr;

	Urho3D::Node* objectNode = LGeneralMethods::GetObjectNode(someNode, objectID);
	if (!objectNode)
		return nullptr;

	return objectNode->GetComponent<Owner>();
}

LTeam * LGeneralMethods::GetOwnerTeam(Urho3D::Node * someNode, U32 objectID)
{
	Node* ownerNode = GetOwnerNode(someNode, objectID);
	if (!ownerNode)
		return nullptr;

	return ownerNode->GetParentComponent<LTeam>(true);
}
LTeam * LGeneralMethods::GetOwnerTeam(Urho3D::Node * objectNode)
{
	Node* ownerNode = GetOwnerNode(objectNode);
	if (!ownerNode)
		return nullptr;

	return ownerNode->GetParentComponent<LTeam>(true);
}

U32 LGeneralMethods::GetOwnerTeamNumber(Urho3D::Node * someNode, U32 objectID)
{
	LTeam* ownerTeam = GetOwnerTeam(someNode, objectID);
	if (ownerTeam)
		ownerTeam->GetTeamNumber();

	return LTeam::NoTeamId;
}

U32 LGeneralMethods::GetOwnerTeamNumber(Urho3D::Node * objectNode)
{
	LTeam* ownerTeam = GetOwnerTeam(objectNode);
	if (ownerTeam)
		ownerTeam->GetTeamNumber();

	return LTeam::NoTeamId;
}

U32 LGeneralMethods::GetOwnerID(Urho3D::Node * someNode, U32 objectID)
{
	Owner* owner = GetOwner(someNode, objectID);
	if (!owner)
		return -1;

	return owner->GetOwnerID();
}

U32 LGeneralMethods::GetOwnerID(Urho3D::Node* objectNode)
{
	if (!objectNode)
		return -1;

	Owner* owner = objectNode->GetComponent<Owner>();
	if (!owner)
		return -1;

	return owner->GetOwnerID();
}

Urho3D::Node * LGeneralMethods::GetOwnerNode(Urho3D::Node * objectNode)
{
	return LGeneralMethods::GetObjectNode(objectNode, LGeneralMethods::GetOwnerID(objectNode));
}

Urho3D::Node * LGeneralMethods::GetOwnerNode(Urho3D::Node * someNode, U32 objectID)
{
	return LGeneralMethods::GetObjectNode(someNode, LGeneralMethods::GetOwnerID(someNode, objectID));
}

void LGeneralMethods::ShowCost(UIElement * element, Urho3D::Node* managerNode, LGameResource::ResourceType costType, F32 cost)
{
	if (!element || !managerNode || costType == LGameResource::ResourceType::Count)
		return;

	ToolTip* buttonToolTip = element->CreateChild<ToolTip>();
	buttonToolTip->SetStyleAuto();

	Text* costText = buttonToolTip->CreateChild<Text>();
	costText->SetStyleAuto();
	costText->SetFont(costText->GetFont(), 20);
	costText->SetText(String(static_cast<U32>(cost)));

	LResourceManager* manager = managerNode->GetComponent<LResourceManager>();
	if (manager && manager->HasResource(costType, cost))
		costText->SetColor(Color::GREEN);
	else
		costText->SetColor(Color::RED);
}

Texture2D* LGeneralMethods::GetTextureFromNode(const Node* node)
{
	if (!node)
		return nullptr;

	StaticSprite2D* staticSprite = node->GetComponent<StaticSprite2D>();
	if (!staticSprite)
		return nullptr;

	Sprite2D* sprite = staticSprite->GetSprite();
	if (!sprite)
		return nullptr;

	return sprite->GetTexture();
}
