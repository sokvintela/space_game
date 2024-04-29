#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include "legacyMode/utility/lSharedData.h"
#include "stateManager/playersManager.h"
#include "legacyMode/galaxy/lStarSystem.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/playerLogic/lControlObject.h"
#include "legacyMode/managers/lDataManager.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/playerLogic/lTeam.h"
#include "prefabs/prefabsStorage.h"
#include "legacyMode/spaceship/lSpaceship.h"

#include "legacyMode/galaxy/galaxy.h"

static const char* galaxyTypeNames[] = {
	"elliptic",
	"spiral2",
	0
};

static const char* galaxyAgeNames[] = {
	"young",
	"medium",
	"old",
	0
};

static const StringHash starSystemsStr = StringHash("starSystems");
static const StringHash playersStr = StringHash("players");
static const StringHash teamsStr = StringHash("teams");

static const U32 aiPoolIdStart = 1000000;

void Galaxy::RegisterObject(Context * context)
{
	context->RegisterFactory<Galaxy>();

	URHO3D_ATTRIBUTE("step", F32, step_, M_PI / 10.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("spiralCoeff", F32, spiralCoeff_, 100.0f, AM_DEFAULT);

	URHO3D_ENUM_ATTRIBUTE("type", type_, galaxyTypeNames, Spiral2, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("age", age_, galaxyAgeNames, Medium, AM_DEFAULT);
	URHO3D_ATTRIBUTE("stars", S32, numberOfStars_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("players", S32, numberOfPlayers_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("galaxyRadius", F32, galaxyRadius_, 0, AM_DEFAULT);
}

Galaxy::Galaxy(Context * context) :
	LogicComponent(context)
{
	step_ = M_PI / 10.0f;
	spiralCoeff_ = 2000.0f;
}

void Galaxy::Generate(Type type, Age age, U32 numberOfStars)
{
	type_ = type;
	age_ = age;
	numberOfStars_ = numberOfStars;

	switch (type_)
	{
	case Type::Elliptic:
		// TODO
	case Type::Spiral2:
		galaxyRadius_ = step_ * spiralCoeff_ * numberOfStars;
		break;
	}

	MarkNetworkUpdate();

	Node* starSystemsNode = node_->CreateChild("starSystems");
	for (U32 i = 0; i < numberOfStars; i++)
	{
		Node* newSystemNode = starSystemsNode->CreateChild(i, REPLICATED);

		Vector2 position = GenerateSystemPosition(i);
		newSystemNode->SetPosition2D(position);

		LStarSystem* starSystem = newSystemNode->CreateComponent<LStarSystem>();
		starSystem->GenerateSystemContent();

		// TODO: choose only suitable systems
		if(starSystem->GetEnergyStar())
			startingStarSystems_.Push(i);
	}
}

Vector2 Galaxy::GenerateSystemPosition(U32 number) const
{
	switch (type_)
	{
		case Galaxy::Type::Elliptic:
		{
			// TODO: not available yet
		}
		case Galaxy::Type::Spiral2:
		{
			// TODO: randomize
			F32 angle = number * step_ + (number % 2 ? M_PI : 0);
			F32 radius = spiralCoeff_ * (angle - (number % 2 ? M_PI : 0));

			return Vector2(radius * cosf(angle), radius * sinf(angle));
		}
		default:
			return Vector2::ZERO;
	}
}

void Galaxy::CreatePlayers(U32 numberAi)
{
	PlayersManager* playersManager = GetSubsystem<PlayersManager>();
	LSharedData* sharedData = GetSubsystem<LSharedData>();

	// workaround for single player
	if (!playersManager)
	{
		playersManager = new PlayersManager(context_);
		context_->RegisterSubsystem(playersManager);
	}

	if(playersManager->connectedPlayers_.Empty())
		playersManager->connectedPlayers_[0] = PlayersManager::ConnectedPlayer{ 0, 0, nullptr };

	numberOfPlayers_ = playersManager->connectedPlayers_.Size() + numberAi;

	// TODO: support more than 2 teams
	U32 numOfTeams = sharedData->teamsCount_;
	Node* teamsNode = node_->CreateChild("teams");
	for (U32 i = 0; i < numOfTeams; i++)
	{
		Node* teamNode = teamsNode->CreateChild("team" + String(i));

		LTeam* team = teamNode->CreateComponent<LTeam>();
		team->Create(i, static_cast<Races::Races>(i));

		teamNode->CreateChild("players");
	}

	for (auto connectedPlayer : playersManager->connectedPlayers_)
	{
		CreatePlayer(connectedPlayer.second_.clientId, connectedPlayer.second_.teamId);
	}

	// TODO ai teaming
	for (U32 i = aiPoolIdStart; i < aiPoolIdStart + numberAi; i++)
	{
		CreatePlayer(i, numOfTeams, /* isAI = */ true);
	}
}

void Galaxy::CreatePlayer(U32 playerNumber, U32 teamNumber, bool isAI)
{
	Node* teamsNode = node_->GetChild(teamsStr);
	if (!teamsNode)
		return;

	Node* teamNode = teamsNode->GetChild("team" + String(teamNumber));
	if (!teamNode)
		return;

	Node* playersNode = teamNode->GetChild(playersStr);
	if (!playersNode)
		return;

	Node* playerNode = playersNode->CreateChild(String(playerNumber));

	playerNode->CreateComponent<LSpaceShip>();

	RigidBody2D* mRigid = playerNode->CreateComponent<RigidBody2D>();
	mRigid->SetBodyType(BT_DYNAMIC);
	mRigid->SetAngularDamping(100.0f);
	mRigid->SetLinearDamping(1.0f);

	CollisionBox2D* boxBase = playerNode->CreateComponent<CollisionBox2D>();
	// TODO: radius
	boxBase->SetSize(1, 1);
	boxBase->SetDensity(1.0f);
	boxBase->SetFriction(0.5f);
	boxBase->SetRestitution(0.1f);

	LDataManager* dManager = playerNode->CreateComponent<LDataManager>();
	dManager->Create();
	dManager->SetClientID(playerNumber);
	dManager->SetMyObjectName("Friendly Spaceship");
	dManager->SetEnemyObjectName("Enemy Spaceship");

	LResourceManager* resource = playerNode->CreateComponent<LResourceManager>();
	resource->Create();
	resource->AddResourceDevice(LGameResource::ResourceDevice::Receiver, LGameResource::ResourceType::Energy, 10000.f);
	resource->AddResourceDevice(LGameResource::ResourceDevice::Receiver, LGameResource::ResourceType::Metals, 300.f);

	LEquipmentManager* equipment = playerNode->CreateComponent<LEquipmentManager>();
	equipment->Create();
	
	// modules
	LSharedData* sharedData = GetSubsystem<LSharedData>();

	// engine
	equipment->AddElementToModule(Modules::Module::M_EnginePlace, LEquipment::EquipmentType::ET_Engine, sharedData->engine_);

	// weapon
	equipment->AddElementToModule(Modules::Module::M_FirstWeaponPlace, LEquipment::ET_Weapon, sharedData->leftWeapon_);
	equipment->AddElementToModule(Modules::Module::M_SecondWeaponPlace, LEquipment::ET_Weapon, sharedData->rightWeapon_);

	if (!isAI)
	{
		playerNode->CreateComponent<LControlObject>();

		// TODO fix races for other human players
		dManager->SetRace(sharedData->myRace_);
		startingRaces_.Insert(sharedData->myRace_);
	}
	else
	{
		//TODO: utilize startingRaces_
		dManager->SetRace(static_cast<Races::Races>(Random(static_cast<int>(Races::Races::R_Count) - 1)));
	}

	StaticSprite2D* sSprite = playerNode->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/spaceships/ship1_11.png"));

	SelectStartSystemForPlayer(playerNode);

	/*
	if (isAI)
	{
		Node* zeroPlayerNode = playersNode->GetChild(0u);
		playerNode->SetWorldPosition2D(zeroPlayerNode->GetWorldPosition2D());
		SpaceShip* ship = playerNode->GetComponent<SpaceShip>(true);
		ship->GetNode()->SetWorldPosition2D(zeroPlayerNode->GetWorldPosition2D() + 2.f * Vector2(playerNumber, playerNumber));
	}
	*/

	MarkNetworkUpdate();
}

void Galaxy::SelectStartSystemForPlayer(Node* playerNode)
{
	if (!playerNode)
		return;

	Node* starSystemsNode = node_->GetChild(starSystemsStr);

	// TODO: fix signed unsigned uncertainty

	S32 starSystemIndexInVector = Random(static_cast<int>(startingStarSystems_.Size()));

	U32 starSystemIndex = startingStarSystems_[starSystemIndexInVector];
	Node* starSystemNode = starSystemsNode->GetChild(starSystemIndex);

	// Real ID of star system on scene
	U32 starSystemId = starSystemNode->GetID();

	LDataManager* dManager = playerNode->GetComponent<LDataManager>();
	if (dManager)
		dManager->SetCurrentStarSystemID(starSystemId);

	LStarSystem* starSystem = starSystemNode->GetComponent<LStarSystem>();
	if (!starSystem)
		return;

	// TODO get size
	Vector2 posInStarSystem = starSystem->GeneratePlayerPosition(10);
	playerNode->SetWorldPosition2D(posInStarSystem);

	startingStarSystems_.EraseSwap(starSystemIndexInVector);
}

Node* Galaxy::GetPlayersNode(U32 teamID) const
{
	Node* teamsNode = node_->GetChild(teamsStr);
	if (!teamsNode)
		return nullptr;

	Node* teamNode = teamsNode->GetChild("team" + String(teamID));
	if (!teamNode)
		return nullptr;

	return teamNode->GetChild(playersStr);
}
