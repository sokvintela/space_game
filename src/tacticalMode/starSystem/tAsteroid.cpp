#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/Sprite2D.h>

#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/managers/tDataManager.h"
#include "tacticalMode/spaceship/tDurability.h"

#include "tacticalMode/starSystem/tAsteroid.h"

using namespace Urho3D;

static const F32 mathPi = static_cast<F32>(M_PI);

void TAsteroid::RegisterObject(Context * context)
{
	context->RegisterFactory<TAsteroid>();
}

TAsteroid::TAsteroid(Context* context) :
	LogicComponent(context)
{
}

TAsteroid::~TAsteroid()
{
}

void TAsteroid::Create(Type type) const
{
	RigidBody2D* mRigid = node_->CreateComponent<RigidBody2D>();
	mRigid->SetBodyType(BT_STATIC);
	mRigid->SetAngularDamping(100000.0f);
	mRigid->SetLinearDamping(100000.0f);

	CollisionCircle2D* colCircle = node_->CreateComponent<CollisionCircle2D>();
	// TODO: radius
	colCircle->SetRadius(1.0f);
	colCircle->SetDensity(1.0f);
	colCircle->SetFriction(0.5f);
	colCircle->SetRestitution(0.1f);

	F32 value = 0.f;
	switch (type)
	{
	case Type::Small:
	{
		value = Random(500.f, 800.f);
		break;
	}
	case Type::Big:
	{
		value = Random(1500.f, 2000.f);
		break;
	}
	}

	TResourceManager* rManager = node_->CreateComponent<TResourceManager>();
	rManager->Create();
	rManager->AddResource(TGameResource::ResourceType::Metals, value);

	TDataManager* dManager = node_->CreateComponent<TDataManager>();
	dManager->SetObjectName("Asteroid");
	dManager->infoWindow_.Push(String(static_cast<U32>(value)) + " metals");

	TDurability* durability = node_->CreateComponent<TDurability>();
	durability->SetDurability(value / 10.f);

	StaticSprite2D* sprite = node_->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/asteroid.png"));

	node_->SetScale2D(Vector2(value, value) * 0.01f);
}
