#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>

#include "tacticalMode/playerLogic/tTeam.h"
#include "tacticalMode/managers/tEquipmentManager.h"
#include "equipment/shipEngine.h"
#include "tacticalMode/managers/tDataManager.h"
#include "tacticalMode/managers/tResourceManager.h"
#include "utility/sharedData.h"
#include "tacticalMode/equipment/weapon/weaponInterface.h"
#include "tacticalMode/starSystem/tStar.h"
#include "tacticalMode/starSystem/tStarSystem.h"

#include "utility/generalMethods.h"

using namespace Urho3D;

static const F32 mathPi = static_cast<F32>(M_PI);

bool GeneralMethods::IsMainPlayer(Urho3D::Node * playerNode)
{
	TDataManager* dManager = playerNode->GetComponent<TDataManager>();
	if (dManager && dManager->GetClientID() == playerNode->GetSubsystem<SharedData>()->clientId_)
		return true;

	return false;
}

Urho3D::Node* GeneralMethods::GetMyControlNode(Urho3D::Node* someNode)
{
	SharedData* sharedData = someNode->GetSubsystem<SharedData>();
	return GetClientNode(someNode, sharedData->clientId_, sharedData->myTeam_);
}

Urho3D::Node* GeneralMethods::GetClientNode(Urho3D::Node * someNode, U32 clientId, U32 teamNumber)
{
	Node* playersNode = GetPlayersNode(someNode, teamNumber);
	if (playersNode)
		return playersNode->GetChild(String(clientId));

	return nullptr;
}

bool GeneralMethods::HasTeam(Urho3D::Node * objectNode)
{
	if (GeneralMethods::GetTeamNumber(objectNode) == -1)
		return false;

	return true;
}

bool GeneralMethods::IsFriendlyTeam(Urho3D::Node * objectNode)
{
	if (GeneralMethods::GetTeamNumber(objectNode) == objectNode->GetSubsystem<SharedData>()->myTeam_)
		return true;
		
	return false;
}

bool GeneralMethods::IsFriendlyTeam(Urho3D::Node * objectNode, Urho3D::Node * myNode)
{
	if (GeneralMethods::GetTeamNumber(objectNode) == GeneralMethods::GetTeamNumber(myNode))
		return true;

	return false;
}

TTeam * GeneralMethods::GetTeam(Urho3D::Node * someNode, U32 teamNumber)
{
	Node* teamNode = GetTeamNode(someNode, teamNumber);
	if (teamNode)
		return teamNode->GetComponent<TTeam>();

	return nullptr;
}

Urho3D::Node * GeneralMethods::GetTeamNode(Urho3D::Node * someNode, U32 teamNumber)
{
	if (!someNode)
		return nullptr;

	Scene* scene = someNode->GetScene();
	if (!scene)
		return nullptr;

	TStarSystem* starSystem = scene->GetComponent<TStarSystem>(true);
	if (!starSystem)
		return nullptr;

	HashSet<U32> teamIDs = starSystem->teamIDs_;

	for (U32 teamID : teamIDs)
	{
		Urho3D::Node* teamNode = GetObjectNode(someNode, teamID);
		if (!teamNode)
			continue;

		TTeam* team = teamNode->GetComponent<TTeam>();
		if (team && team->GetTeamNumber() == teamNumber)
			return teamNode;
	}

	return nullptr;
}

U32 GeneralMethods::GetTeamNumber(Urho3D::Node * myNode)
{
	if (!myNode)
		return TTeam::NoTeamId;

	TDataManager* dManager = myNode->GetComponent<TDataManager>();
	if (dManager)
		return dManager->GetTeamNumber();

	return TTeam::NoTeamId;
}

U32 GeneralMethods::GetTeamNumber(Urho3D::Node * someNode, S32 objectID)
{
	return GetTeamNumber(GetObjectNode(someNode, objectID));
}

Urho3D::Node * GeneralMethods::GetMyTeamNode(Urho3D::Node * someNode)
{
	return GetTeamNode(someNode, someNode->GetSubsystem<SharedData>()->myTeam_);
}

Urho3D::Node * GeneralMethods::GetObjectNode(Urho3D::Node * someNode, S32 objectID)
{
	if (!someNode || objectID <= 0)
		return nullptr;

	Urho3D::Scene* scene = someNode->GetScene();
	if (scene)
		return scene->GetNode(objectID);

	return nullptr;
}

bool GeneralMethods::MoveObject(Node * objectNode, Vector2 unnormilizedDirection, F32 timeStep, F32 limitDistance)
{
	if (!objectNode)
		return false;

	if (unnormilizedDirection.Length() < limitDistance)
		return true;

	TEquipmentManager* manager = objectNode->GetComponent<TEquipmentManager>();
	if (!manager)
		return false;

	ShipEngine* engine = manager->GetElement<ShipEngine>(TEquipmentManager::Module::M_Engine);
	RigidBody2D* rigidBody = objectNode->GetComponent<RigidBody2D>();
	if (rigidBody && engine)
	{
		unnormilizedDirection.Normalize();
		rigidBody->ApplyForceToCenter(unnormilizedDirection * engine->GetSpeed() * timeStep * 10.0f, true);
		return true;
	}

	return false;
}

bool GeneralMethods::MoveObject(Urho3D::Node * objectNode, Urho3D::Node * targetNode, F32 limitDistance)
{
	if (!objectNode || !targetNode)
		return false;

	Vector2 direction = targetNode->GetWorldPosition2D() - objectNode->GetWorldPosition2D();
	if (direction.Length() < limitDistance)
		return true;

	direction.Normalize();
	return MoveObject(objectNode, direction, limitDistance);
}

bool GeneralMethods::MoveObject(Urho3D::Node * objectNode, U32 targetID, F32 limitDistance)
{
	if (!objectNode)
		return false;

	Node* targetNode = GeneralMethods::GetObjectNode(objectNode, targetID);
	if (!targetNode)
		return false;

	if ((targetNode->GetWorldPosition2D() - objectNode->GetWorldPosition2D()).Length() < 1.f)
		return true;

	return MoveObject(objectNode, targetNode, limitDistance);
}

Vector2 GeneralMethods::GetObjectBias(Urho3D::Node* myNode, U32 objectID)
{
	Urho3D::Node* objectNode = GeneralMethods::GetObjectNode(myNode, objectID);
	if (!objectNode)
		return Vector2();

	RigidBody2D* rugid = objectNode->GetComponent<RigidBody2D>();
	if (!rugid)
		return Vector2();

	Vector2 objectVelocity = rugid->GetLinearVelocity();
	F32 objectSpeed = objectVelocity.Length();

	WeaponInterface* weapon = myNode->GetDerivedComponent<WeaponInterface>(true);
	if (!weapon)
		return Vector2();

	F32 bulletSpeed = weapon->GetAttackSpeed();
	F32 distance = (myNode->GetWorldPosition2D() - objectNode->GetWorldPosition2D()).Length();
	Vector2 bias = objectVelocity * (distance * (objectSpeed / (std::sqrt(objectSpeed * objectSpeed + bulletSpeed * bulletSpeed))));

	return objectNode->GetWorldPosition2D() + bias;
}

void GeneralMethods::ShowCost(UIElement * element, Urho3D::Node* managerNode, TGameResource::ResourceType costType, F32 cost)
{
	if (!element || !managerNode || costType == TGameResource::ResourceType::Count)
		return;

	ToolTip* buttonToolTip = element->CreateChild<ToolTip>();
	buttonToolTip->SetStyleAuto();

	Text* costText = buttonToolTip->CreateChild<Text>();
	costText->SetStyleAuto();
	costText->SetFont(costText->GetFont(), 20);
	costText->SetText(String(static_cast<U32>(cost)));

	TResourceManager* manager = managerNode->GetComponent<TResourceManager>();
	if (manager && manager->HasResource(costType, cost))
		costText->SetColor(Color::GREEN);
	else
		costText->SetColor(Color::RED);
}

Texture2D* GeneralMethods::GetTextureFromNode(const Node* node)
{
	if (!node)
		return nullptr;

	StaticSprite2D* staticSprite = node->GetComponent<StaticSprite2D>();
	if (!staticSprite)
		return nullptr;

	Sprite2D* sprite = staticSprite->GetSprite();
	if (sprite)
		return sprite->GetTexture();

	return nullptr;
}

IntVector2 GeneralMethods::WorldToScreen(Urho3D::Camera* camera, Urho3D::Vector3 worldCoords)
{
	Vector2 screenPoint = camera->WorldToScreenPoint(worldCoords);

	Graphics* graphics = camera->GetSubsystem<Graphics>();
	if (!graphics)
		return IntVector2::ZERO;

	Vector2 screenScaled = Vector2(screenPoint.x_ * graphics->GetWidth(), screenPoint.y_ * graphics->GetHeight());
	return IntVector2(static_cast<S32>(screenScaled.x_), static_cast<S32>(screenScaled.y_));
}

Vector2 GeneralMethods::GetMouseWorldPositionXY(Urho3D::Camera* camera)
{
	Input* input = camera->GetSubsystem<Input>();
	Graphics* graphics = camera->GetSubsystem<Graphics>();
	if (!graphics || !camera)
		return Vector2::ZERO;

	Vector3 screenPoint = Vector3((F32)input->GetMousePosition().x_ / graphics->GetWidth(), (F32)input->GetMousePosition().y_ / graphics->GetHeight(), 0.0f);
	Vector3 worldPoint = camera->ScreenToWorldPoint(screenPoint);
	return Vector2(worldPoint.x_, worldPoint.y_);
}

U32 GeneralMethods::GetOppositeTeamNumber(U32 teamNum)
{
	return (teamNum == 0 ? 1 : 0);
}

U32 GeneralMethods::GetOppositeTeamNumber(Urho3D::Node * myNode)
{
	return (GetTeamNumber(myNode) == 0 ? 1 : 0);
}

Urho3D::Node * GeneralMethods::GetPlayersNode(Urho3D::Node * someNode, U32 teamNumber)
{
	Node* teamNode = GetTeamNode(someNode, teamNumber);
	if (teamNode)
		return teamNode->GetChild("players");

	return nullptr;
}
