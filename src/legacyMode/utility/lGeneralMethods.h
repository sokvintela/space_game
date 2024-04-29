#pragma once

#include <Urho3D/Scene/Node.h>

#include "legacyMode/galaxy/spaceObjects/lPlanet.h"
#include "legacyMode/playerLogic/lControlObject.h"
#include "legacyMode/resources/lGameResource.h"
#include "legacyMode/buildings/lBuilding.h"
#include "legacyMode/playerLogic/owner.h"
#include "legacyMode/playerLogic/lTeam.h"

class LResourceManager;
class LStarSystem;
class Owner;
class GameResource;
class LDataManager;

namespace Urho3D
{
	class UIElement;
	class Texture2D;
};

namespace LGeneralMethods
{
	Urho3D::Vector2 GeneratePositionInCircle(F32 circleRadius);

	Urho3D::Node* GetMyControlNode(Urho3D::Node* someNode);
	Urho3D::Node* GetClientNode(Urho3D::Node* someNode, U32 clientId, U32 teamNumber);
	Urho3D::Node* GetObjectNode(Urho3D::Node* someNode, S32 objectID);

	bool IsMainPlayer(Urho3D::Node* playerNode);
	LTeam* GetTeam(Urho3D::Node* someNode, U32 teamNumber);
	Urho3D::Node* GetTeamNode(Urho3D::Node* someNode, U32 teamNumber);
	U32 GetTeamNumber(Urho3D::Node* myNode);
	U32 GetTeamNumber(Urho3D::Node* someNode, S32 objectID);
	Urho3D::Node* GetMyTeamNode(Urho3D::Node* someNode);

	Owner* GetOwner(Urho3D::Node* someNode, U32 objectID);
	U32 GetOwnerID(Urho3D::Node* someNode, U32 objectID);
	U32 GetOwnerID(Urho3D::Node* objectNode);
	Urho3D::Node* GetOwnerNode(Urho3D::Node* objectNode);
	Urho3D::Node* GetOwnerNode(Urho3D::Node* someNode, U32 objectID);

	LTeam* GetOwnerTeam(Urho3D::Node* someNode, U32 objectID);
	LTeam* GetOwnerTeam(Urho3D::Node* objectNode);
	U32 GetOwnerTeamNumber(Urho3D::Node* someNode, U32 objectID);
	U32 GetOwnerTeamNumber(Urho3D::Node* objectNode);

	Urho3D::Node* GetStarNode(Urho3D::Node* playerNode);
	LStarSystem* GetStarSystem(Urho3D::Node* someNode, S32 systemID);

	Urho3D::Node* GetCurrentStarSystemNode(Urho3D::Node* objectNode);
	Urho3D::Node* GetCurrentStarSystemNode(Urho3D::Node* someNode, U32 objectID);
	LStarSystem* GetCurrentStarSystem(Urho3D::Node* objectNode);
	LStarSystem* GetCurrentStarSystem(Urho3D::Node* someNode, U32 objectID);

	Urho3D::PODVector<Urho3D::Node*> GetShipNodes(Urho3D::Node* someNode);
	Urho3D::PODVector<Urho3D::Node*> GetStarSystemNodes(Urho3D::Node* someNode);

	Urho3D::PODVector<Urho3D::Node*> GetPlanetNodes(Urho3D::Node* someNode, U32 systemID, LPlanet::PlanetType type = LPlanet::PlanetType::PT_Count);
	Urho3D::PODVector<Urho3D::Node*> GetPlanetNodes(Urho3D::Node* systemNode, LPlanet::PlanetType type = LPlanet::PlanetType::PT_Count);

	LResourceManager* GetPlanetBuildingResourceManager(Urho3D::Node* planetNode, String buildingName);
	LResourceManager* GetPlanetBuildingResourceManager(Urho3D::Node* someNode, U32 planetID, String buildingName);

	Urho3D::Node* GetClosestPlanetNode(Urho3D::Node* myNode, LPlanet::PlanetType type = LPlanet::PlanetType::PT_Count);
	Urho3D::Vector2 GetClosestPlanetPosition(Urho3D::Node* myNode, LPlanet::PlanetType type = LPlanet::PlanetType::PT_Count);

	LDataManager* GetOwnerDataManager(Urho3D::Node* objectNode);
	LDataManager* GetOwnerDataManager(Urho3D::Node* someNode, U32 objectID);

	bool MoveObject(Urho3D::Node* objectNode, Urho3D::Vector2 direction, F32 limitDistance = 1.f);
	bool MoveObject(Urho3D::Node* objectNode, Urho3D::Node* targetNode, F32 limitDistance = 1.f);
	bool MoveObject(Urho3D::Node* objectNode, U32 targetID, F32 limitDistance = 1.f);

	LBuilding::TargetType GetObjectTargetType(Urho3D::Node* objectNode);
	void ShowCost(UIElement * element, Urho3D::Node* managerNode, LGameResource::ResourceType costType, F32 cost);

	Urho3D::Texture2D* GetTextureFromNode(const Node* node);

	template<typename T>
	T * GetObjectManager(Urho3D::Node * someNode, unsigned objectID)
	{
		Node* objectNode = LGeneralMethods::GetObjectNode(someNode, objectID);
		if (!objectNode)
			return nullptr;

		return objectNode->GetComponent<T>();
	}
}
