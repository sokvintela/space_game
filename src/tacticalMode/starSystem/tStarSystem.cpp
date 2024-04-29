#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/MemoryBuffer.h>

#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>

#include "tacticalMode/playerLogic/tTeam.h"
#include "tacticalMode/starSystem/tAsteroid.h"
#include "prefabs/prefabsStorage.h"
#include "tacticalMode/starSystem/tStar.h"
#include "tacticalMode/starSystem/tPlanet.h"

#include "tacticalMode/starSystem/tStarSystem.h"

static const F32 mathPi = static_cast<F32>(M_PI);

static const Pair<F32, F32> asteroidBeltZone = { 800.0f, 1500.0f };
static const Pair<S32, S32> smallAsteroidsNumRange = { 100, 300 };
static const Pair<S32, S32> bigAsteroidsNumRange = { 50, 150 };

static const Vector<Pair<F32, F32>> planetZones =
{
	{ 1800.0f, 2000.0f },
	{ 2300.0f, 2600.0f },
	{ 3000.0f, 3500.0f }
};

static Pair<F32, F32> planetSizeRange = { 30.0f, 60.0f };
static Pair<F32, F32> planetOrbitAngleRange = { -mathPi / 8.0f, mathPi / 8.0f };


void TStarSystem::RegisterObject(Context * context)
{
	context->RegisterFactory<TStarSystem>();

	URHO3D_ATTRIBUTE("starSystemRadius", F32, starSystemRadius_, 0.f, AM_DEFAULT);

	URHO3D_ATTRIBUTE("starId", U32, starID_, 0, AM_NET);
	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("asteroidIds", GetAsteroidIdsAttr, SetAsteroidIdsAttr, PODVector<U8>, Variant::emptyBuffer, AM_NET);
	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("planetIds", GetPlanetIdsAttr, SetPlanetIdsAttr, PODVector<U8>, Variant::emptyBuffer, AM_NET);
	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("teamIds", GetTeamIdsAttr, SetTeamIdsAttr, PODVector<U8>, Variant::emptyBuffer, AM_NET);
}

TStarSystem::TStarSystem(Context * context)
	: LogicComponent(context)
	, starSystemRadius_(0.f)
	, starID_(0)
{
}

void TStarSystem::SetAsteroidIdsAttr(const PODVector<U8>& value)
{
	if (value.Empty())
		return;

	MemoryBuffer buffer(value);
	while (!buffer.IsEof())
	{
		U32 asteroidId = buffer.ReadUInt();
		asteroidIDs_.Insert(asteroidId);
	}

	MarkNetworkUpdate();
}

PODVector<U8> TStarSystem::GetAsteroidIdsAttr() const
{
	VectorBuffer ret;

	for (auto asteroidId : asteroidIDs_)
	{
		ret.WriteUInt(asteroidId);
	}

	return ret.GetBuffer();
}

void TStarSystem::SetPlanetIdsAttr(const PODVector<U8>& value)
{
	if (value.Empty())
		return;

	MemoryBuffer buffer(value);
	while (!buffer.IsEof())
	{
		U32 planetId = buffer.ReadUInt();
		planetIDs_.Insert(planetId);
	}

	MarkNetworkUpdate();
}

PODVector<U8> TStarSystem::GetPlanetIdsAttr() const
{
	VectorBuffer ret;

	for (auto planetId : planetIDs_)
	{
		ret.WriteUInt(planetId);
	}

	return ret.GetBuffer();
}

void TStarSystem::SetTeamIdsAttr(const PODVector<U8>& value)
{
	if (value.Empty())
		return;

	MemoryBuffer buffer(value);
	while (!buffer.IsEof())
	{
		U32 teamId = buffer.ReadUInt();
		teamIDs_.Insert(teamId);
	}

	MarkNetworkUpdate();
}

PODVector<U8> TStarSystem::GetTeamIdsAttr() const
{
	VectorBuffer ret;

	for (auto teamId : teamIDs_)
	{
		ret.WriteUInt(teamId);
	}

	return ret.GetBuffer();
}

void TStarSystem::GenerateSystemContent()
{
	CreateTeams();
	CreateStar();
	CreateAsteroidBelt();

	Vector<F32> radiuses;

	for (U32 i = 0; i < planetZones.Size(); i++)
	{
		F32 radius = Random(planetZones[i].first_, planetZones[i].second_);
		F32 angle = Random(planetOrbitAngleRange.first_, planetOrbitAngleRange.second_);
		F32 size = Random(planetSizeRange.first_, planetSizeRange.second_);

		CreatePlanet(size, radius, angle, 1);
		CreatePlanet(size, radius, mathPi + angle, 0);

		if (radius > starSystemRadius_)
		{
			starSystemRadius_ = radius;
			MarkNetworkUpdate();
		}
	}
}

void TStarSystem::CreateTeams()
{
	// TODO: support more than 2 teams
	U32 numOfTeams = 2;
	Node* teamsNode = node_->CreateChild("teams");
	for (U32 i = 0; i < numOfTeams; i++)
	{
		Node* teamNode = teamsNode->CreateChild("team" + String(i));
		teamsNode->AddChild(teamNode);
		teamIDs_.Insert(teamNode->GetID());

		TTeam* team = teamNode->CreateComponent<TTeam>();
		team->Create(i, static_cast<Races::Races>(i));
	}
}

void TStarSystem::CreateStar()
{
	//Node* starNode = GetSubsystem<PrefabsStorage>()->GetStar();
	//node_->AddChild(starNode);

	// TODO : get from prefabs
	//ResourceCache* cache = GetSubsystem<ResourceCache>();
	//XMLFile* starXML = cache->GetResource<XMLFile>("Prefabs/SpaceObjects/Star.xml");
	//Node* starNode = node_->GetScene()->InstantiateXML(starXML->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
	//node_->AddChild(starNode);

	Node* starNode = node_->CreateChild("star");
	starID_ = starNode->GetID();

	TStar* star = starNode->CreateComponent<TStar>();
	star->Create();

	Node* forNavigationNode = node_->GetScene()->GetChild("ForNavigation");
	Node* obstacleNode = forNavigationNode->CreateChild("ObstacleStar");
	obstacleNode->SetScale(100.0f);
	StaticModel* obstacleMesh = obstacleNode->CreateComponent<StaticModel>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	obstacleMesh->SetModel(cache->GetResource<Model>("Models/Obstacle.mdl"));
	obstacleMesh->SetMaterial(cache->GetResource<Material>("Materials/DefaultMaterial.xml"));
}

void TStarSystem::CreateAsteroidBelt()
{
	U32 sAsteroidNum = Random(smallAsteroidsNumRange.first_, smallAsteroidsNumRange.second_);
	U32 bAsteroidNum = Random(bigAsteroidsNumRange.first_, bigAsteroidsNumRange.second_);

	for (U32 i = 0; i < sAsteroidNum + bAsteroidNum; i++)
	{
		//Node* asteroidNode = GetSubsystem<PrefabsStorage>()->GetAsteroid();
		//node_->AddChild(asteroidNode);

		// TODO : get from prefabs
		/*
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		XMLFile* asteroidXML = cache->GetResource<XMLFile>("Prefabs/SpaceObjects/Asteroid.xml");
		Node* asteroidNode = node_->GetScene()->InstantiateXML(asteroidXML->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
		node_->AddChild(asteroidNode);
		*/

		TAsteroid::Type type = TAsteroid::Type::Small;
		if (i > sAsteroidNum)
			type = TAsteroid::Type::Big;

		Node* asteroidNode = node_->CreateChild("asteroid");
		asteroidIDs_.Insert(asteroidNode->GetID());

		F32 radius = Random(asteroidBeltZone.first_, asteroidBeltZone.second_);
		F32 angle = Random(0.f, 2.f * mathPi);

		asteroidNode->SetPosition2D(radius * std::cos(angle), radius * std::sin(angle));

		TAsteroid* asteroid = asteroidNode->CreateComponent<TAsteroid>();
		asteroid->Create(type);
		F32 scale = 10.0f; // TODO: get asteroid scale

		Node* forNavigationNode = node_->GetScene()->GetChild("ForNavigation");
		Node* obstacleNode = forNavigationNode->CreateChild("ObstacleAsteroid");
		obstacleNode->SetScale(scale);
		StaticModel* obstacleMesh = obstacleNode->CreateComponent<StaticModel>();

		Vector3 obstaclePosition = Vector3(radius * std::cos(angle), 0, radius * std::sin(angle));
		obstacleNode->Translate(obstaclePosition);

		ResourceCache* cache = GetSubsystem<ResourceCache>();
		obstacleMesh->SetModel(cache->GetResource<Model>("Models/Obstacle.mdl"));
		obstacleMesh->SetMaterial(cache->GetResource<Material>("Materials/DefaultMaterial.xml"));
	}
}

void TStarSystem::CreatePlanet(F32 size, F32 radius, F32 angle, U32 teamNumber)
{
	//Node* planetNode = GetSubsystem<PrefabsStorage>()->GetPlanet();
	//planetNode->SetScale(size);
	//node_->AddChild(planetNode);

	// TODO : get from prefabs
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* planetXML = cache->GetResource<XMLFile>("Prefabs/SpaceObjects/Planet.xml");
	Node* planetNode = node_->GetScene()->InstantiateXML(planetXML->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
	node_->AddChild(planetNode);

	planetNode->SetScale(size);
	planetIDs_.Insert(planetNode->GetID());

	TPlanet* planet = planetNode->CreateComponent<TPlanet>();
	planet->Create(radius, angle, teamNumber);

	Vector2 position = planetNode->GetPosition2D();

	Node* forNavigationNode = node_->GetScene()->GetChild("ForNavigation");
	Node* obstacleNode = forNavigationNode->CreateChild("ObstaclePlanet");
	obstacleNode->SetScale(size);
	StaticModel* obstacleMesh = obstacleNode->CreateComponent<StaticModel>();

	Vector3 obstaclePosition = Vector3(position.x_, 0, position.y_);
	obstacleNode->Translate(obstaclePosition);

	obstacleMesh->SetModel(cache->GetResource<Model>("Models/Obstacle.mdl"));
	obstacleMesh->SetMaterial(cache->GetResource<Material>("Materials/DefaultMaterial.xml"));
}
