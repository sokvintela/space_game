#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Core/Profiler.h>

#include "equipment/shipEngine.h"
#include "tacticalMode/equipment/weapon/weapon.h"
#include "tacticalMode/managers/tEquipmentManager.h"
#include "tacticalMode/managers/tDataManager.h"
#include "prefabs/prefabsStorage.h"
#include "utility/generalMethods.h"
#include "tacticalMode/starSystem/tBaseShip.h"
#include "tacticalMode/spaceship/tDurability.h"
#include "tacticalMode/playerLogic/tTeam.h"

#include "tacticalMode/ai/tAINodeLogic.h"

#define URHO3D_PROFILE_NODE(name, node) Urho3D::AutoProfileBlock profile_ ## name (node->GetSubsystem<Urho3D::Profiler>(), #name)

namespace AI
{
namespace Nodes
{

std::unique_ptr<AINode> FlyToEnemyBase::Clone() const
{
	auto newNode = std::make_unique<FlyToEnemyBase>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus FlyToEnemyBase::Process(F32 timeStep)
{
	U32 enemyTeam = GeneralMethods::GetOppositeTeamNumber(GeneralMethods::GetTeamNumber(aiElement_));
	Node* enemyTeamNode = GeneralMethods::GetTeamNode(aiElement_, enemyTeam);
	if (!enemyTeamNode)
		return NodeReturnStatus::Failure;

	TBaseShip* baseShip = enemyTeamNode->GetComponent<TBaseShip>(true);
	if (!baseShip)
		return NodeReturnStatus::Failure;

	Vector2 direction = baseShip->GetNode()->GetWorldPosition2D() - aiElement_->GetWorldPosition2D();
	if (direction.Length() < 10.0f)
		return NodeReturnStatus::Success;

	if (GeneralMethods::MoveObject(aiElement_, direction, timeStep))
		return NodeReturnStatus::Running;

	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> IsEnemyAround::Clone() const
{
	auto newNode = std::make_unique<IsEnemyAround>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus IsEnemyAround::Process(F32 timeStep)
{
	URHO3D_PROFILE_NODE(AI_IsEnemyAround, aiElement_);

	TStarSystem* starSystem = GeneralMethods::GetOnlyComponent<TStarSystem>(aiElement_);
	TTeam* team = GeneralMethods::GetTeam(aiElement_, GeneralMethods::GetOppositeTeamNumber(aiElement_));
	if (!starSystem || !team)
		return NodeReturnStatus::Failure;

	Vector<Node*> enemyNodes;

	HashSet<U32> planetIDs = starSystem->planetIDs_;
	for (U32 planetID : planetIDs)
	{
		Node* objectNode = GeneralMethods::GetObjectNode(aiElement_, planetID);
		if (!GeneralMethods::IsFriendlyTeam(objectNode, aiElement_))
			enemyNodes.Push(objectNode);
	}

	enemyNodes.Push(GeneralMethods::GetObjectNode(aiElement_, team->baseShipID_));

	HashSet<U32> playerIDs = team->playerIDs_;
	for (U32 playerID : playerIDs)
		enemyNodes.Push(GeneralMethods::GetObjectNode(aiElement_, playerID));

	HashSet<U32> mobIDs = team->mobIDs_;
	for (U32 mobID : mobIDs)
		enemyNodes.Push(GeneralMethods::GetObjectNode(aiElement_, mobID));

	TDataManager* myDManager = aiElement_->GetComponent<TDataManager>();
	if (!myDManager)
		return NodeReturnStatus::Failure;

	F32 visibilityArea = myDManager->GetVisibilityArea();

	for (U32 i = 0; i < enemyNodes.Size(); i++)
	{
		if (GeneralMethods::IsFriendlyTeam(enemyNodes[i], aiElement_)
			|| GeneralMethods::GetTeamNumber(enemyNodes[i]) == -1)
			continue;

		Vector2 direction = enemyNodes[i]->GetWorldPosition2D() - aiElement_->GetWorldPosition2D();
		if (direction.Length() < visibilityArea)
		{
			myDManager->SetTargetID(enemyNodes[i]->GetID());
			return NodeReturnStatus::Success;
		}
	}

	return NodeReturnStatus::Failure;
}

std::unique_ptr<AINode> Stop::Clone() const
{
	auto newNode = std::make_unique<Stop>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus Stop::Process(F32 timeStep)
{
	TDataManager* dManager = aiElement_->GetComponent<TDataManager>();
	if (!dManager)
		return NodeReturnStatus::Failure;

	RigidBody2D* ridgid = aiElement_->GetComponent<RigidBody2D>();
	if (!ridgid)
		return NodeReturnStatus::Failure;

	Vector2 velocity = ridgid->GetLinearVelocity();
	if (velocity.Length() > 1.f)
	{
		TEquipmentManager* manager = aiElement_->GetComponent<TEquipmentManager>();
		if (!manager)
			return NodeReturnStatus::Failure;

		ShipEngine* engine = manager->GetElement<ShipEngine>(TEquipmentManager::Module::M_Engine);
		if (!engine)
			return NodeReturnStatus::Failure;

		velocity.Normalize();
		ridgid->ApplyForceToCenter(velocity * dManager->GetBreakingForceCoeff() * engine->GetSpeed(), true);
		return NodeReturnStatus::Running;
	}
	
	return NodeReturnStatus::Success;
}

std::unique_ptr<AINode> Fire::Clone() const
{
	auto newNode = std::make_unique<Fire>();
	newNode->childNodes_ = this->CloneChildren();
	return std::unique_ptr<AINode>(std::move(newNode));
}

NodeReturnStatus Fire::Process(F32 timeStep)
{
	TEquipmentManager* eManager = aiElement_->GetComponent<TEquipmentManager>();
	TDataManager* dManager = aiElement_->GetComponent<TDataManager>();
	if (!eManager || !dManager)
		return NodeReturnStatus::Failure;

	Vector<Node*> weaponNodes = eManager->GetElementNodes(TEquipment::EquipmentType::ET_Weapon);
	for (Node* weaponNode : weaponNodes)
	{
		Weapon* weapon = weaponNode->GetComponent<Weapon>();
		weapon->Shoot(GeneralMethods::GetObjectBias(aiElement_, dManager->GetTargetID()));
		return NodeReturnStatus::Success;
	}

	return NodeReturnStatus::Failure;
}

};
};
