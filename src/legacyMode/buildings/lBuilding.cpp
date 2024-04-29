#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>

#include "legacyMode/managers/lBuildingsManager.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/galaxy/spaceObjects/lPlanet.h"
#include "legacyMode/galaxy/spaceObjects/lStar.h"
#include "legacyMode/galaxy/spaceObjects/celestialBody.h"
#include "legacyMode/equipment/lEquipment.h"

#include "legacyMode/buildings/lBuilding.h"

void LBuilding::RegisterObject(Context* context)
{
	context->RegisterFactory<LBuilding>();

	URHO3D_ENUM_ATTRIBUTE("landingType", landingType_, landingTypeNames, LBuilding::LandingType::LT_Count, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("targetType", targetType_, targetTypeNames, LBuilding::TargetType::TT_Count, AM_DEFAULT);
	URHO3D_ENUM_ACCESSOR_ATTRIBUTE("technology", GetTechnologyType, SetTechnologyType, Research::ResearchType, researchTypeNames, Research::ResearchType::Base, AM_DEFAULT);
}

LBuilding::LBuilding(Context* context) :
	LogicComponent(context)
{
	landingType_ = LandingType::LT_Count;
	targetType_ = TargetType::TT_Count;
}

LBuilding::LandingType LBuilding::GetLandingType() const
{
	return landingType_;
}

LBuilding::TargetType LBuilding::GetTargetType() const
{
	return targetType_;
}

Research::ResearchType LBuilding::GetTechnologyType() const
{
	return technology_;
}

void LBuilding::SetTechnologyType(Research::ResearchType type)
{
	technology_ = type;
	MarkNetworkUpdate();
}

void LBuilding::Create(String buildingName, TargetType targetType)
{
	targetType_ = targetType;
	MarkNetworkUpdate();

	LStar* star = node_->GetParentComponent<LStar>(true);
	LPlanet* planet = node_->GetParentComponent<LPlanet>(true);
	LBuildingsManager* bManager = node_->GetParentComponent<LBuildingsManager>(true);

	if (buildingName == "radar")
	{
		LResourceManager* rManager = node_->CreateComponent<LResourceManager>();
		rManager->Create();
		rManager->AddResourceDevice(LGameResource::ResourceDevice::Receiver, LGameResource::ResourceType::Energy);
	}
	else if (buildingName == "defenderPlatform")
	{
		StaticSprite2D* sSprite = node_->CreateComponent<StaticSprite2D>();

		ResourceCache* cache = GetSubsystem<ResourceCache>();
		sSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/probes/probe"));

		CelestialBody* celestialBody = node_->CreateComponent<CelestialBody>();
		celestialBody->SetRadius(5.f);
		celestialBody->SetAngle(50.f);
		celestialBody->SetAngularSpped(250.f);

		RigidBody2D* platformRigid = node_->CreateComponent<RigidBody2D>();
		platformRigid->SetBodyType(BT_DYNAMIC);
		platformRigid->SetAngularDamping(100.f);

		CollisionCircle2D* circle = node_->CreateComponent<CollisionCircle2D>();
		circle->SetRadius(0.3f);
		circle->SetFriction(0.5f);
		circle->SetRestitution(0.1f);
		//circle->SetDensity(1.0f);

		if (!bManager)
			return;

		node_->SetWorldPosition2D(Vector2(6.f, 8.f) * 5.f + bManager->GetNode()->GetWorldPosition2D());
		node_->SetScale2D(0.15f, 0.15f);

		LEquipmentManager* eManager = node_->CreateComponent<LEquipmentManager>();
		eManager->AddElementToModule(Modules::Module::M_FirstWeaponPlace, LEquipment::ET_Weapon, "MachineGun");

	}
	else if (buildingName == "solarStation")
	{
		StaticSprite2D* sSprite = node_->CreateComponent<StaticSprite2D>();

		ResourceCache* cache = GetSubsystem<ResourceCache>();
		sSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/probes/probe"));

		CelestialBody* celestialBody = node_->CreateComponent<CelestialBody>();
		celestialBody->SetRadius(10.f);
		celestialBody->SetAngle(0.f);
		celestialBody->SetAngularSpped(250.f);

		RigidBody2D* platformRigid = node_->CreateComponent<RigidBody2D>();
		platformRigid->SetBodyType(BT_DYNAMIC);
		platformRigid->SetAngularDamping(100.f);

		CollisionCircle2D* circle = node_->CreateComponent<CollisionCircle2D>();
		circle->SetRadius(0.3f);
		circle->SetFriction(0.5f);
		circle->SetRestitution(0.1f);

		if (!bManager)
			return;

		node_->SetPosition2D(Vector2(0.6f, 0.8f) * 5.f + bManager->GetNode()->GetWorldPosition2D());
		node_->SetScale2D(0.6f, 0.6f);
	}
	else if (buildingName == "scienceLaboratory")
	{
		if (!star || star->IsEnergyStar())
			return;

		LResourceManager* manager = star->GetComponent<LResourceManager>();
		if (manager)
			manager->AddResourceDevice(LGameResource::ResourceDevice::Generator, LGameResource::ResourceType::Knowledge);
	}
	else if (buildingName == "metalExtractionFactory")
	{
		if (!planet || planet->GetPlanetType() != LPlanet::PT_Solid)
			return;

		LResourceManager* manager = planet->GetComponent<LResourceManager>();
		if (manager)
			manager->AddResourceDevice(LGameResource::ResourceDevice::Generator, LGameResource::ResourceType::Metals);
	}
}
