#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Network/Network.h>

#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/managers/tEquipmentManager.h"
#include "prefabs/prefabsStorage.h"
#include "tacticalMode/managers/tDataManager.h"
#include "tacticalMode/starSystem/tBaseShip.h"
#include "stateManager/playersManager.h"
#include "tacticalMode/ai/mob.h"
#include "tacticalMode/playerLogic/tControlObject.h"
#include "utility/sharedData.h"
#include "tacticalMode/spaceship/tEnergyShield.h"
#include "tacticalMode/spaceship/tDurability.h"
#include "ai/AIBase.h"
#include "tacticalMode/spaceship/tSpaceship.h"

#include "tacticalMode/playerLogic/tTeam.h"

static const U32 aiPoolIdStart = 1000000;
static const F32 mobCreationInterval = 10000.f;

static const StringHash playersStr = StringHash("players");
static const StringHash teamsStr = StringHash("teams");

void TTeam::RegisterObject(Context* context)
{
	context->RegisterFactory<TTeam>();

	URHO3D_ATTRIBUTE("number", U32, number_, 0, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("race", race_, Races::raceNames_, Races::R_Count, AM_DEFAULT);

	URHO3D_ATTRIBUTE("baseShipId", U32, baseShipID_, 0, AM_NET);
	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("playerIds", GetPlayerIdsAttr, SetPlayerIdsAttr, PODVector<U8>, Variant::emptyBuffer, AM_NET);
	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("mobIds", GetMobIdsAttr, SetMobIdsAttr, PODVector<U8>, Variant::emptyBuffer, AM_NET);
}

TTeam::TTeam(Context * context) :
	LogicComponent(context),
	number_(0),
	race_(Races::R_Count)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

TTeam::~TTeam()
{
}

void TTeam::Create(U32 teamNumber, Races::Races race)
{
	// team creation
	number_ = teamNumber;
	race_ = race;
	MarkNetworkUpdate();

	// add resource receiver
	TResourceManager* rManager = node_->CreateComponent<TResourceManager>();
	rManager->Create();

	// create base ship
	CreateBaseShip();

	// create team players
	CreatePlayers(0);
}

U32 TTeam::GetTeamNumber() const
{
	return number_;
}

void TTeam::CreateBaseShip()
{
	//Node* baseShipNode = GetSubsystem<PrefabsStorage>()->GetBaseShip();
	//node_->AddChild(baseShipNode);

	// TODO : get from prefabs
	/*
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* baseShipXML = cache->GetResource<XMLFile>("Prefabs/Ships/BaseShip.xml");
	Node* baseShipNode = node_->GetScene()->InstantiateXML(baseShipXML->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
	node_->AddChild(baseShipNode);
	*/

	Node* baseShipNode = node_->CreateChild("baseShip");
	baseShipID_ = baseShipNode->GetID();

	RigidBody2D* mRigid = baseShipNode->CreateComponent<RigidBody2D>();
	mRigid->SetBodyType(BT_DYNAMIC);
	mRigid->SetAngularDamping(100000.0f);
	mRigid->SetLinearDamping(100000.0f);

	CollisionBox2D* colBox = baseShipNode->CreateComponent<CollisionBox2D>();
	// TODO: radius
	colBox->SetSize(2.0f, 2.0f);
	colBox->SetDensity(1.0f);
	colBox->SetFriction(0.5f);
	colBox->SetRestitution(0.1f);

	TBaseShip* baseShip = baseShipNode->CreateComponent<TBaseShip>();
	baseShip->Create();

	F32 xBasePosition = (number_ == 0 ? -5000.0f : 5000.0f);
	baseShipNode->SetPosition2D(xBasePosition, 0);

	baseShipNode->CreateComponent<TEnergyShield>();

	TDurability* durability = baseShipNode->CreateComponent<TDurability>();
	durability->SetDurability(2000.f);
	durability->SetMaxDurability(2000.f);

	//baseShipNode->CreateComponent<GravityAffected>();

	TDataManager* dManager = baseShipNode->CreateComponent<TDataManager>();
	dManager->SetTeamNumber(number_);
	dManager->SetObjectName("BaseShip");
	dManager->SetVisibilityArea(200.f);

	TEquipmentManager* equipment = baseShipNode->CreateComponent<TEquipmentManager>();
	equipment->Create();
	equipment->AddElementToModule(TEquipmentManager::Module::M_FirstWeapon, GetSubsystem<PrefabsStorage>()->GetAvailabeEquipment(TEquipment::ET_Weapon, nullptr).Begin()->first_, true);

	AIBase* aiBase = baseShipNode->CreateComponent<AIBase>(LOCAL);
	aiBase->SetBehavior("AI/AIBaseShip.xml");
	aiBase->Activate(true);

	StaticSprite2D* sprite = baseShipNode->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/ships/baseShip.png"));
}

void TTeam::CreatePlayers(U32 numberAi)
{
	node_->CreateChild("players");

	PlayersManager* playersManager = GetSubsystem<PlayersManager>();
	if (!playersManager)
	{
		playersManager = new PlayersManager(context_);
		context_->RegisterSubsystem(playersManager);
	}

	numberOfPlayers_ = playersManager->connectedPlayers_.Size() + numberAi;

	// workaround for single player
	if (playersManager->connectedPlayers_.Empty())
		playersManager->connectedPlayers_[0] = PlayersManager::ConnectedPlayer{ 0, 0, nullptr };

	for (auto connectedPlayer : playersManager->connectedPlayers_)
	{
		if (connectedPlayer.second_.teamId == number_)
			CreatePlayer(connectedPlayer.second_.clientId, false);
	}

	// TODO ai teaming
	for (U32 i = aiPoolIdStart; i < aiPoolIdStart + numberAi; i++)
	{
		CreatePlayer(i, /* isAI = */ true);
	}
}

void TTeam::CreatePlayer(U32 playerNumber, bool isAI)
{
	Node* playersNode = node_->GetChild(playersStr);
	if (!playersNode)
		return;

	//Node* playerNode = GetSubsystem<PrefabsStorage>()->GetPlayer();

	// TODO : get from prefabs
	/*
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* playerXml = cache->GetResource<XMLFile>("Prefabs/PlayersLogic/Player.xml");
	Node* playerNode = node_->GetScene()->InstantiateXML(playerXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
	playerNode->SetName(String(playerNumber));
	playersNode->AddChild(playerNode);
	*/

	Node* playerNode = playersNode->CreateChild("player");
	playerNode->SetName(String(playerNumber));
	playerIDs_.Insert(playerNode->GetID());

	RigidBody2D* mRigid = playerNode->CreateComponent<RigidBody2D>();
	mRigid->SetBodyType(BT_DYNAMIC);
	mRigid->SetAngularDamping(100.0f);
	mRigid->SetLinearDamping(0.01f);
	mRigid->SetInertia(0.0f);

	CollisionBox2D* box = playerNode->CreateComponent<CollisionBox2D>();
	// TODO: radius
	box->SetSize(1.0f, 1.0f);
	box->SetDensity(1.0f);
	box->SetFriction(0.5f);
	box->SetRestitution(0.1f);

	playerNode->CreateComponent<TSpaceShip>();
	//playerNode->CreateComponent<GravityAffected>();

	TEnergyShield* shield = playerNode->CreateComponent<TEnergyShield>();

	TDurability* durability = playerNode->CreateComponent<TDurability>();
	durability->SetDurability(2000.f);
	durability->SetMaxDurability(2000.f);

	TDataManager* dManager = playerNode->CreateComponent<TDataManager>();
	dManager->SetClientID(playerNumber);
	dManager->SetTeamNumber(number_);
	dManager->SetObjectName("Spaceship");
	dManager->SetVisibilityArea(200.f);

	TResourceManager* resource = playerNode->CreateComponent<TResourceManager>();
	resource->Create();
	resource->AddResourceDevice(TGameResource::ResourceDevice::Receiver, TGameResource::ResourceType::Energy, 10000.f);
	resource->AddResourceDevice(TGameResource::ResourceDevice::Receiver, TGameResource::ResourceType::Metals, 300.f);

	TEquipmentManager* equipment = playerNode->CreateComponent<TEquipmentManager>();
	equipment->Create();

	// modules
	SharedData* sharedData = GetSubsystem<SharedData>();

	// engine
	equipment->AddElementToModule(TEquipmentManager::Module::M_Engine, sharedData->engine_);

	// weapon
	equipment->AddElementToModule(TEquipmentManager::Module::M_FirstWeapon, sharedData->leftWeapon_);
	equipment->AddElementToModule(TEquipmentManager::Module::M_SecondWeapon, sharedData->rightWeapon_);

	if (!isAI)
	{
		TControlObject* controlObject = playerNode->CreateComponent<TControlObject>();

		// TODO fix races for other human players
		dManager->SetRace(sharedData->myRace_);
		startingRaces_.Insert(sharedData->myRace_);
	}
	else
	{
		//TODO: utilize startingRaces_
		dManager->SetRace(static_cast<Races::Races>(Random(static_cast<int>(Races::Races::R_Count) - 1)));
	}

	/*
	if (isAI)
	{
	Node* zeroPlayerNode = playersNode->GetChild(0u);
	playerNode->SetWorldPosition2D(zeroPlayerNode->GetWorldPosition2D());
	SpaceShip* ship = playerNode->GetComponent<SpaceShip>(true);
	ship->GetNode()->SetWorldPosition2D(zeroPlayerNode->GetWorldPosition2D() + 2.f * Vector2(playerNumber, playerNumber));
	}
	*/

	// TODO get size
	TBaseShip* baseShip = node_->GetComponent<TBaseShip>(true);
	if (!baseShip)
		return;

	Vector2 posInStarSystem = baseShip->GetNode()->GetWorldPosition2D();//GeneratePlayerPosition(10);
	playerNode->SetWorldPosition2D(posInStarSystem + Vector2(Random(10.f, 15.f), Random(10.f, 15.f)));

	StaticSprite2D* sprite = playerNode->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/spaceships/ship1_11.png"));

	MarkNetworkUpdate();
}

void TTeam::SetPlayerIdsAttr(const PODVector<U8>& value)
{
	if (value.Empty())
		return;

	MemoryBuffer buffer(value);
	while (!buffer.IsEof())
	{
		U32 playerId = buffer.ReadUInt();
		playerIDs_.Insert(playerId);
	}

	MarkNetworkUpdate();
}

PODVector<U8> TTeam::GetPlayerIdsAttr() const
{
	VectorBuffer ret;

	for (auto playerId : playerIDs_)
	{
		ret.WriteUInt(playerId);
	}

	return ret.GetBuffer();
}

void TTeam::SetMobIdsAttr(const PODVector<U8>& value)
{
	if (value.Empty())
		return;

	MemoryBuffer buffer(value);
	while (!buffer.IsEof())
	{
		U32 mobId = buffer.ReadUInt();
		mobIDs_.Insert(mobId);
	}

	MarkNetworkUpdate();
}

PODVector<U8> TTeam::GetMobIdsAttr() const
{
	VectorBuffer ret;

	for (auto mobId : mobIDs_)
	{
		ret.WriteUInt(mobId);
	}

	return ret.GetBuffer();
}

void TTeam::Update(F32 timestep)
{
	if (mobCreationTimer_.GetMSec(false) > mobCreationInterval)
	{
		TBaseShip* baseShip = node_->GetComponent<TBaseShip>(true);
		if (!baseShip)
			return;

		Node* systemNode = node_->GetParent();
		Vector2 baseShipPos = baseShip->GetNode()->GetPosition2D();
		PrefabsStorage* prefabs = GetSubsystem<PrefabsStorage>();
		ResourceCache* cache = GetSubsystem<ResourceCache>();

		for (S32 i = -1; i < 2; i++)
		{
			//Node* mobNode = prefabs->CreateMob();

			// TODO : get from prefabs
			XMLFile* mobXml = cache->GetResource<XMLFile>("Prefabs/Ships/Mob.xml");
			Node* mobNode = node_->GetScene()->InstantiateXML(mobXml->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
			mobNode->SetPosition2D(baseShipPos.x_ + (number_ == 0 ? 10 : -10), baseShipPos.y_ + (number_ == 0 ? 10 * i : -10 * i));
			systemNode->AddChild(mobNode);

			mobIDs_.Insert(mobNode->GetID());

			Mob* mob = mobNode->CreateComponent<Mob>();
			if (mob)
				mob->Create(0, number_);
		}

		mobCreationTimer_.Reset();
		MarkNetworkUpdate();
	}
}
