#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionBox2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/playerLogic/lResearchTree.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/playerLogic/owner.h"
#include "prefabs/prefabsStorage.h"
#include "legacyMode/managers/lDataManager.h"
#include "ai/AIBase.h"
#include "legacyMode/playerLogic/lBaseShip.h"
#include "stateManager/playersManager.h"
#include "legacyMode/utility/lGeneralMethods.h"

#include "legacyMode/playerLogic/lTeam.h"

static const U32 aiPoolIdStart = 1000000;

static const StringHash playersStr = StringHash("players");
static const StringHash teamsStr = StringHash("teams");

void LTeam::RegisterObject(Context* context)
{
	context->RegisterFactory<LTeam>();

	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("foundSystems", GetFoundStarSystemsAttr, SetFoundStarSystemsAttr, 
		PODVector<U8>, Variant::emptyBuffer, AM_DEFAULT);
	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("surveyedSystems", GetSurveyedStarSystemsAttr, SetSurveyedStarSystemsAttr, 
		PODVector<U8>, Variant::emptyBuffer, AM_DEFAULT);

	URHO3D_ENUM_ATTRIBUTE("race", race_, Races::raceNames_, Races::R_Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("number", U32, number_, 0, AM_DEFAULT);
}

LTeam::LTeam(Context * context) :
	LogicComponent(context),
	number_(0)
{
}

LTeam::~LTeam()
{
}

void LTeam::SetSurveyedStarSystemsAttr(const PODVector<U8>& value)
{
	if (value.Empty())
		return;

	MemoryBuffer buffer(value);
	while (!buffer.IsEof())
	{
		U32 starSystemId = buffer.ReadUInt();
		U32 planetsSize = buffer.ReadUInt();
		StarSystemInfo planets;
		for (U32 planetNum = 0; planetNum < planetsSize; planetNum++)
		{
			planets.surveyedPlanets_.Insert(buffer.ReadUInt());
		}

		surveyedStarSystems_[starSystemId] = planets;
	}

	MarkNetworkUpdate();
}

PODVector<U8> LTeam::GetSurveyedStarSystemsAttr() const
{
	VectorBuffer ret;

	for (auto starSystemIt = surveyedStarSystems_.Begin(); starSystemIt != surveyedStarSystems_.End(); ++starSystemIt)
	{
		ret.WriteUInt(starSystemIt->first_);
		ret.WriteUInt(starSystemIt->second_.surveyedPlanets_.Size());
		for (auto planetIt = starSystemIt->second_.surveyedPlanets_.Begin(); planetIt != starSystemIt->second_.surveyedPlanets_.End(); ++planetIt)
		{
			ret.WriteUInt(*planetIt);
		}
	}

	return ret.GetBuffer();
}

void LTeam::SetFoundStarSystemsAttr(const PODVector<U8>& value)
{
	if (value.Empty())
		return;

	MemoryBuffer buffer(value);
	while (!buffer.IsEof())
	{
		U32 starSystemId = buffer.ReadUInt();
		AddFoundStarSystem(starSystemId);
	}
}

PODVector<U8> LTeam::GetFoundStarSystemsAttr() const
{
	VectorBuffer ret;

	for (auto starSystemIt = foundStarSystems_.Begin(); starSystemIt != foundStarSystems_.End(); ++starSystemIt)
		ret.WriteUInt(*starSystemIt);

	return ret.GetBuffer();
}

void LTeam::Create(U32 teamId, Races::Races race)
{
	number_ = teamId;

	//XMLFile* shipPrefabFile = cache->GetResource<XMLFile>("Data/Prefabs/Ships/MainShip.xml");
	//Node* spaceShipNode = scene->InstantiateXML(shipPrefabFile->GetRoot(), node_->GetPosition(), node_->GetRotation());

	// base ship
	Node* baseShipNode = node_->CreateChild("baseShip");

	F32 xBasePosition = (number_ == 0 ? -100.0f : 100.0f);
	baseShipNode->SetPosition2D(xBasePosition, 0);

	baseShipNode->CreateComponent<LBaseShip>();

	RigidBody2D* mRigid = baseShipNode->CreateComponent<RigidBody2D>();
	mRigid->SetBodyType(BT_DYNAMIC);
	mRigid->SetAngularDamping(100.0f);

	CollisionBox2D* box = baseShipNode->CreateComponent<CollisionBox2D>();
	// TODO: radius
	box->SetSize(2.0f, 2.f);
	box->SetDensity(1.0f);
	box->SetFriction(0.5f);
	box->SetRestitution(0.1f);

	StaticSprite2D* sSprite = baseShipNode->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/ships/baseShip.png"));

	//DataManager* dManager = baseShipNode->CreateComponent<DataManager>();
	//dManager->Create();
	//dManager->SetRace(GetSubsystem<SharedData>()->myRace_);

	LEquipmentManager* equipment = baseShipNode->CreateComponent<LEquipmentManager>();
	equipment->Create();
	equipment->AddElementToModule(Modules::Module::M_FirstWeaponPlace, LEquipment::ET_Weapon, GetSubsystem<PrefabsStorage>()->GetAvailabeLegacyEquipment(LEquipment::ET_Weapon, nullptr).Begin()->first_);

	AIBase* aiBaseShip = baseShipNode->CreateComponent<AIBase>();
	aiBaseShip->SetBehavior("AI/AIBaseShip.xml");

	node_->CreateChild("players");
	node_->CreateChild("probes");

	node_->CreateComponent<LResearchTree>();

	race_ = race;
	MarkNetworkUpdate();

	LResourceManager* rManager = node_->CreateComponent<LResourceManager>();
	rManager->Create();
	rManager->AddResourceDevice(LGameResource::ResourceDevice::Receiver, LGameResource::ResourceType::Knowledge, 100.f);
}

void LTeam::AddFoundStarSystem(U32 id)
{
	if (!foundStarSystems_.Contains(id))
	{
		foundStarSystems_.Insert(id);
		MarkNetworkUpdate();
	}
}

void LTeam::AddSurveyedStarSystem(U32 id)
{
	AddFoundStarSystem(id);

	if (!surveyedStarSystems_.Contains(id))
	{
		StarSystemInfo info;

		PODVector<Node*> planetNodes = LGeneralMethods::GetPlanetNodes(node_, id);
		for (U32 i = 0; i < planetNodes.Size(); i++)
			info.surveyedPlanets_.Insert(planetNodes[i]->GetID());

		surveyedStarSystems_[id] = info;
	}

	MarkNetworkUpdate();
}

bool LTeam::IsStarSystemFound(U32 id) const
{
	if (foundStarSystems_.Contains(id))
		return true;

	return false;
}

bool LTeam::IsStarSystemSurveyed(U32 id) const
{
	if (surveyedStarSystems_.Contains(id))
		return true;

	return false;
}

bool LTeam::IsPlanetSurveyed(U32 starSystemID, U32 planetID)
{
	if (surveyedStarSystems_.Contains(starSystemID) &&
		surveyedStarSystems_[starSystemID].surveyedPlanets_.Contains(planetID))
		return true;

	return false;
}

U32 LTeam::GetTeamNumber() const
{
	return number_;
}

Node * LTeam::GetProbesNode() const
{
	return node_->GetChild("probes");
}

void LTeam::AddProbe()
{
	// probes
	Node* probesNode = GetProbesNode();
	if (!probesNode)
		return;

	// probe
	PrefabsStorage* prefabsStorage = GetSubsystem<PrefabsStorage>();
	Node* probeNode = prefabsStorage->CreateProbe(node_);
	if (!probeNode)
		return;

	LDataManager* dManager = probeNode->CreateComponent<LDataManager>();
	dManager->Create();
	dManager->SetMyObjectName("Friendly Research Probe");
	dManager->SetEnemyObjectName("Enemy Research Probe");

	LEquipmentManager* eManager = probeNode->CreateComponent<LEquipmentManager>();
	eManager->Create();

	U32 probeID = probesNode->GetNumChildren();
	probesNode->AddChild(probeNode, probeID);

	// owner
	Owner* probeOwner = probeNode->CreateComponent<Owner>();
	probeOwner->SetOwnerID(node_->GetID());

	AIBase* aiBase = node_->GetComponent<AIBase>();
	aiBase->Activate(true);
}
