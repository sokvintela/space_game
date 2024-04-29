#pragma once

#include <Urho3D/Scene/Scene.h>

#include "tacticalMode/starSystem/tPlanet.h"
#include "tacticalMode/resources/tGameResource.h"
#include "tacticalMode/starSystem/tStarSystem.h"

class TTeam;

namespace Urho3D
{
	class UIElement;
	class Texture2D;
	class Camera;
};

namespace GeneralMethods
{
	Urho3D::Node* GetMyControlNode(Urho3D::Node* someNode);
	Urho3D::Node* GetClientNode(Urho3D::Node* someNode, U32 clientId, U32 teamNumber);
	Urho3D::Node* GetObjectNode(Urho3D::Node* someNode, S32 objectID);

	Urho3D::Node* GetPlayersNode(Urho3D::Node* someNode, U32 teamNumber);
	bool IsMainPlayer(Urho3D::Node* playerNode);

	bool HasTeam(Urho3D::Node* objectNode);
	TTeam* GetTeam(Urho3D::Node* someNode, U32 teamNumber);

	bool IsFriendlyTeam(Urho3D::Node* objectNode);
	bool IsFriendlyTeam(Urho3D::Node* objectNode, Urho3D::Node* myNode);

	Urho3D::Node* GetTeamNode(Urho3D::Node* someNode, U32 teamNumber);
	Urho3D::Node* GetMyTeamNode(Urho3D::Node* someNode);

	U32 GetTeamNumber(Urho3D::Node* myNode);
	U32 GetTeamNumber(Urho3D::Node* someNode, S32 objectID);

	U32 GetOppositeTeamNumber(U32 teamNum);
	U32 GetOppositeTeamNumber(Urho3D::Node* myNode);

	bool MoveObject(Urho3D::Node* objectNode, Urho3D::Vector2 unnormilizedDirection, F32 timeStep, F32 limitDistance = 1.f);
	bool MoveObject(Urho3D::Node* objectNode, Urho3D::Node* targetNode, F32 limitDistance = 1.f);
	bool MoveObject(Urho3D::Node* objectNode, U32 targetID, F32 limitDistance = 1.f);

	Vector2 GetObjectBias(Urho3D::Node* myNode, U32 objectID);

	void ShowCost(UIElement * element, Urho3D::Node* managerNode, TGameResource::ResourceType costType, F32 cost);

	Urho3D::Texture2D* GetTextureFromNode(const Node* node);

	IntVector2 WorldToScreen(Urho3D::Camera* camera, Urho3D::Vector3 worldCoords);
	Vector2 GetMouseWorldPositionXY(Urho3D::Camera* camera);

	template<typename T>
	T * GetComponent(Urho3D::Node * someNode, unsigned objectID)
	{
		Urho3D::Node* objectNode = GetObjectNode(someNode, objectID);
		if (objectNode)
			return objectNode->GetComponent<T>();

		return nullptr;
	}

	template<typename T>
	T * GetOnlyComponent(Urho3D::Node * someNode)
	{
		if (!someNode)
			return nullptr;

		Urho3D::Scene* scene = someNode->GetScene();
		if (scene)
			return scene->GetComponent<T>(true);

		return nullptr;
	}

	template<typename T>
	Urho3D::Node * GetOnlyNode(Urho3D::Node * someNode)
	{
		T* component = GetOnlyComponent<T>(someNode);
		if (component)
			return component->GetNode();

		return nullptr;
	}

	template<typename T>
	T * GetObjectManager(Urho3D::Node * someNode, unsigned objectID)
	{
		Node* objectNode = GeneralMethods::GetObjectNode(someNode, objectID);
		if (!objectNode)
			return nullptr;

		return objectNode->GetComponent<T>();
	}

	template<typename T>
	Urho3D::PODVector<Urho3D::Node*> GetNodesWithComponent(Urho3D::Node * someNode)
	{
		PODVector<Urho3D::Node*> nodes;
		if (!someNode)
			return nodes;

		Urho3D::Scene* scene = someNode->GetScene();
		if (!scene)
			return nodes;

		TStarSystem* starSystem = scene->GetComponent<TStarSystem>(true);
		if (!starSystem)
			return nodes;

		starSystem->GetNode()->GetChildrenWithComponent<T>(nodes, true);
		return nodes;
	}
}
