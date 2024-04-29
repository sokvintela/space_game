#include <Urho3D/Core/Context.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>

#include "legacyMode/galaxy/spaceObjects/pulsar.h"
#include "legacyMode/galaxy/spaceObjects/crystal.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/managers/lDataManager.h"

#include "legacyMode/galaxy/lStarSystem.h"

static const F32 mathPi = static_cast<F32>(M_PI);

U32 LStarSystem::crystalGenerationTimeoutMs_ = 1; // 20 seconds
U32 LStarSystem::createAsteroidTimeMS_ = 1000;

void LStarSystem::RegisterObject(Context * context)
{
	context->RegisterFactory<LStarSystem>();

	URHO3D_ATTRIBUTE("starSystemRadius_", F32, starSystemRadius_, 0.f, AM_DEFAULT);
}

LStarSystem::LStarSystem(Context * context) :
	LogicComponent(context),
	starSystemRadius_(0.f)
{
}

void LStarSystem::GenerateSystemContent()
{
	F32 randomValue = Random(0.0f, 1.0f);
	if (randomValue < 0.8f)
		AddStar(LStar::StarType::Count);
	else if (randomValue < 0.9f)
	{
		AddPulsar();
		return;
	}
	else
	{
		AddStar(LStar::StarType::NeutronStar);
		return;
	}

	Vector<F32> radiuses;

	U32 solidPlanetsNum = Random(4) + 1;
	U32 gasGiantsNum = Random(3);

	for (U32 i = 0; i < solidPlanetsNum + gasGiantsNum; i++)
		// generate position
		while (true)
		{
			// TODO : get star size
			F32 radius = Random(100.0f, 200.0f);
			for (U32 i = 0; i < radiuses.Size(); i++)
				if (std::abs(radius - radiuses[i]) < 2.f)
					continue;

			radiuses.Push(radius);
			F32 angle = Random(0.0f, static_cast<F32>(2 * M_PI));

			if (i < solidPlanetsNum)
				AddPlanet(LPlanet::PlanetType::PT_Solid, radius, angle);
			else
				AddPlanet(LPlanet::PlanetType::PT_GasGiant, radius, angle);

			if (radius > starSystemRadius_)
			{
				starSystemRadius_ = radius;
				MarkNetworkUpdate();
			}

			break;
		}
}

Vector2 LStarSystem::GeneratePlayerPosition(S32 playerSize) const
{
	Vector2 position;

	F32 sizes = static_cast<F32>(10 + playerSize);
	PODVector<Node*> planetNodes = LGeneralMethods::GetPlanetNodes(node_);
	if (planetNodes.Empty())
	{
		position = node_->GetPosition2D() + Vector2(sizes, sizes);
	}
	else
	{
		S32 planetIndex = Random(static_cast<S32>(planetNodes.Size()));
		position = planetNodes[planetIndex]->GetPosition2D() + Vector2(sizes, sizes);
	}

	return node_->GetPosition2D() + position;
}

Crystal* LStarSystem::CreateCrystal(bool especial)
{
	Vector2 crystalPos;
	while (true)
	{
		bool truLocation = true;

		// crystal pos
		crystalPos = LGeneralMethods::GeneratePositionInCircle(starSystemRadius_);

		// sun pos
		LStar* star = node_->GetComponent<LStar>(true);
		if (star)
		{
			// TODO sun size
			S32 sunSize = 4;
			if ((crystalPos.x_ > (0.f - sunSize)) && (crystalPos.x_ < (0.f + sunSize))
				&& (crystalPos.y_ >(0.f + sunSize)) && (crystalPos.y_ < (0.f - sunSize)))
			{
				truLocation = false;
				continue;
			}
		}

		// planets pos
		PODVector<Node*> planetNodes = LGeneralMethods::GetPlanetNodes(node_);
		for (U32 i = 0; i < planetNodes.Size(); i++)
		{
			Vector2 planetPos = planetNodes[i]->GetPosition2D();
			// TODO planets size
			S32 planetSize = 4;

			if ((crystalPos.x_ >(planetPos.x_ - planetSize)) && (crystalPos.x_ < (planetPos.x_ + planetSize))
				&& (crystalPos.y_ >(planetPos.y_ + planetSize)) && (crystalPos.y_ < (planetPos.y_ - planetSize)))
			{
				truLocation = false;
				break;
			}
		}

		if (truLocation)
			break;
	}

	Node* crystalNode = node_->CreateChild();
	crystalNode->SetScale(0.05f);
	crystalNode->SetPosition2D(crystalPos);

	Crystal* crystal = crystalNode->CreateComponent<Crystal>();
	crystal->Create();

	crystalGenerationTimer_.Reset();
	MarkNetworkUpdate();

	return crystal;
}

bool LStarSystem::HasCrystal() const
{
	if (node_->HasComponent<Crystal>())
		return true;

	return false;
}

bool LStarSystem::IsCrystalReadyToCreate()
{
	if (!node_->GetComponent<Crystal>() && crystalGenerationTimer_.GetMSec(false) > crystalGenerationTimeoutMs_)
		return true;

	return false;
}

void LStarSystem::AddStar(LStar::StarType type) const
{
	Node* starNode = node_->CreateChild();
	LStar* star = starNode->CreateComponent<LStar>();
	star->Create(type);

	LDataManager* dManager = starNode->GetComponent<LDataManager>();
	if (dManager)
	{
		dManager->Create();
		dManager->SetCurrentStarSystemID(node_->GetID());
	}
}

void LStarSystem::AddPulsar() const
{
	Node* starNode = node_->CreateChild();

	Pulsar* pulsar = starNode->CreateComponent<Pulsar>();
	starNode->SetScale2D(pulsar->GetMaxStarScale() / 4.f);
	pulsar->SetPulsarInitialScale(node_->GetScale2D());

	StaticSprite2D* sSprite = starNode->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/sources/star.png"));
	sSprite->SetColor(Color::WHITE);

	LDataManager* dManager = starNode->CreateComponent<LDataManager>();
	dManager->Create();
	dManager->SetCurrentStarSystemID(node_->GetID());
}

void LStarSystem::AddPlanet(LPlanet::PlanetType type, F32 radius, F32 angle) const
{
	Node* planetNode = node_->CreateChild();
	LPlanet* planet = planetNode->CreateComponent<LPlanet>();
	planet->Create(type, radius, angle);

	LDataManager* dManager = planetNode->GetComponent<LDataManager>();
	if (dManager)
	{
		dManager->Create();
		dManager->SetCurrentStarSystemID(node_->GetID());
	}
}

LStar * LStarSystem::GetEnergyStar() const
{
	LStar* star = node_->GetComponent<LStar>(true);
	if (star && star->IsEnergyStar())
		return star;

	return nullptr;
}

void LStarSystem::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	/*
	if (createAsteroidTimer_.GetMSec(false) > createAsteroidTimeMS_)
	{
		// asteroid
		Node* asteroidNode = node_->CreateChild("asteroid");

		//F32 angle = Random(static_cast<F32>(2 * M_PI));
		//F32 radius = starSystemRadius_ + Random(10.f);

		Vector2 asteroidPos = Vector2(node_->GetWorldPosition2D() + Vector2(50.f, 50.f)); //radius * std::cos(angle), radius * std::sin(angle));
		asteroidNode->SetWorldPosition2D(asteroidPos);

		Asteroid* asteroid = asteroidNode->CreateComponent<Asteroid>();

		Vector2 direction = GeneralMethods::GeneratePositionInCircle(starSystemRadius_) - asteroidPos;
		direction.Normalize();
		asteroid->Create(direction);

		createAsteroidTimer_.Reset();
		MarkNetworkUpdate();
		
		// debris
		Node* debrisNode = node_->CreateChild("debris");
		Debris* debris = debrisNode->CreateComponent<Debris>();
		debris->Create(Vector2(0.f, 0.f));
		debrisNode->SetWorldPosition2D(node_->GetWorldPosition2D() + Vector2(50.f, 50.f));
	}
	*/
}
