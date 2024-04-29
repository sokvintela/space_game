#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>

#include "legacyMode/resources/lGameResource.h"
#include "legacyMode/managers/lDataManager.h"

#include "legacyMode/galaxy/spaceObjects/lAsteroid.h"

using namespace Urho3D;

void LAsteroid::RegisterObject(Context * context)
{
	context->RegisterFactory<LAsteroid>();

	URHO3D_ATTRIBUTE("direction", Vector2, direction_, Vector2(0.f, 0.f), AM_DEFAULT);
	URHO3D_ATTRIBUTE("speed", F32, speed_, 0.f, AM_DEFAULT);
}

LAsteroid::LAsteroid(Context* context) :
	LogicComponent(context)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

LAsteroid::~LAsteroid()
{
}

void LAsteroid::Create(Vector2 direction)
{
	StaticSprite2D* sSprite = node_->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/asteroid.png"));

	RigidBody2D* rigidCrystal = node_->CreateComponent<RigidBody2D>();
	rigidCrystal->SetBodyType(BT_DYNAMIC);
	rigidCrystal->SetAngularDamping(0.0f);

	CollisionCircle2D* circle = node_->CreateComponent<CollisionCircle2D>();
	// TODO: radius
	circle->SetRadius(4.0f);
	circle->SetDensity(1.0f);
	circle->SetFriction(0.5f);
	circle->SetRestitution(0.1f);

	LDataManager* dManager = node_->CreateComponent<LDataManager>();
	dManager->Create();
	dManager->SetMyObjectName("Asteroid");

	F32 value = Random(0.f, 400.f);
	speed_ = Random(1.0f, 10.0f);
	direction_ = direction;

	LGameResource* metals = node_->CreateComponent<LGameResource>();
	metals->AddResource(LGameResource::ResourceType::Metals, value);

	node_->SetScale2D(50.f, 50.f);//Vector2(value, value) * 100.f);
}

void LAsteroid::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	node_->Translate2D(direction_ * speed_ * timeStep);

	// TODO distance
	if (node_->GetWorldPosition2D().Length() > 100.f)
		node_->Remove();
}
