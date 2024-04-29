#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/AnimatedSprite2D.h>
#include <Urho3D/Urho2D/AnimationSet2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>

#include "legacyMode/managers/lBuildingsManager.h"
#include "legacyMode/managers/lDataManager.h"
#include "legacyMode/galaxy/spaceObjects/celestialBody.h"

#include "legacyMode/galaxy/spaceObjects/lPlanet.h"

static const F32 capacityCoeff = 20.0f;

void LPlanet::RegisterObject(Context* context)
{
	context->RegisterFactory<LPlanet>();

	URHO3D_ENUM_ATTRIBUTE("planetType", planetType_, planetTypeNames, PT_Solid, AM_DEFAULT);
	URHO3D_ATTRIBUTE("energyReceptionCapacity", F32, energyReceptionCapacity_, 0.f, AM_DEFAULT);
}

LPlanet::LPlanet(Context * context) :
	LogicComponent(context),
	planetType_(PT_Solid)
{
}

void LPlanet::Create(PlanetType type, F32 radius, F32 angle)
{
	CelestialBody* celestialBody = node_->CreateComponent<CelestialBody>();
	celestialBody->SetRadius(radius);
	celestialBody->SetAngle(angle);
	celestialBody->SetAngularSpped(250.f);

	LDataManager* dManager = node_->CreateComponent<LDataManager>();
	dManager->Create();
	dManager->SetMyObjectName(planetTypeNames[type]);

	F32 size;
	switch (type)
	{
	case PlanetType::PT_GasGiant:
	{
		size = Random(0.5f, 1.0f);

		StaticSprite2D* sSprite = node_->CreateComponent<StaticSprite2D>();

		ResourceCache* cache = GetSubsystem<ResourceCache>();
		sSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/planets/gasGigant.png"));

		break;
	}
	case PlanetType::PT_Solid:
	{
		size = Random(0.5f, 2.0f);
		energyReceptionCapacity_ = size * capacityCoeff;
		dManager->SetObjectEfficiency(Random(1.f, 100.f));
		dManager->infoWindow_.Push("energy reception capacity: " + String(static_cast<U32>(energyReceptionCapacity_)));

		ResourceCache* cache = GetSubsystem<ResourceCache>();
		AnimationSet2D* spriterAnimationSet = cache->GetResource<AnimationSet2D>("Urho2D/earth_anim/earth_anim.scml");
		if (!spriterAnimationSet)
			return;

		AnimatedSprite2D* spriterAnimatedSprite = node_->CreateComponent<AnimatedSprite2D>();
		spriterAnimatedSprite->SetAnimationSet(spriterAnimationSet);
		spriterAnimatedSprite->SetAnimation(spriterAnimationSet->GetAnimation(0));
		spriterAnimatedSprite->SetSpeed(0.05f);

		LBuildingsManager* bManager = node_->CreateComponent<LBuildingsManager>();
		bManager->Create();

		break;
	}
	}

	Vector2 parentScale = node_->GetParent()->GetScale2D();
	node_->SetScale2D(size / parentScale.x_, size / parentScale.y_);

	//RigidBody2D* rigidPlanet = node_->CreateComponent<RigidBody2D>();
	//rigidPlanet->SetBodyType(BT_DYNAMIC);
	//rigidPlanet->SetLinearDamping(10000000000000.f);
	//rigidPlanet->SetAngularDamping(10000000000000.f);

	// TODO: fill out moons
	// moons_
}

LPlanet::PlanetType LPlanet::GetPlanetType() const
{
	return planetType_;
}

F32 LPlanet::GetEnergyReceptionCapacity() const
{
	return energyReceptionCapacity_;
}
