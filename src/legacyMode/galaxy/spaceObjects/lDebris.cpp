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

#include "legacyMode/galaxy/spaceObjects/lDebris.h"

using namespace Urho3D;

void LDebris::RegisterObject(Context * context)
{
	context->RegisterFactory<LDebris>();

	URHO3D_ATTRIBUTE("direction", Vector2, direction_, Vector2(0.f, 0.f), AM_DEFAULT);
	URHO3D_ATTRIBUTE("speed", F32, speed_, 0.f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("metals", F32, metals_, 50.f, AM_DEFAULT);
}

LDebris::LDebris(Context* context) :
	LogicComponent(context),
	metals_(50.f)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

LDebris::~LDebris()
{
}

F32 LDebris::Getmetals() const
{
	return metals_;
}

void LDebris::Create(Vector2 direction)
{
	StaticSprite2D* sSprite = node_->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sSprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/debris.png"));

	RigidBody2D* rigidCrystal = node_->CreateComponent<RigidBody2D>();
	rigidCrystal->SetBodyType(BT_DYNAMIC);
	rigidCrystal->SetAngularDamping(0.0f);

	CollisionCircle2D* circle = node_->CreateComponent<CollisionCircle2D>();
	// TODO: radius
	circle->SetRadius(4.0f);
	circle->SetDensity(1.0f);
	circle->SetFriction(0.5f);
	circle->SetRestitution(0.1f);

	F32 value = Random(0.f, 400.f);
	speed_ = Random(1.0f, 10.0f);
	direction_ = direction;

	LGameResource* metals = node_->CreateComponent<LGameResource>();
	metals->AddResource(LGameResource::ResourceType::Metals, value);

	LDataManager* dManager = node_->CreateComponent<LDataManager>();
	dManager->Create();
	dManager->SetMyObjectName("Debris");
	dManager->infoWindow_.Push(String(static_cast<U32>(value)) + " metals");

	node_->SetScale2D(Vector2(value, value) * 0.001f);
}

void LDebris::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	//node_->Translate2D(direction_ * speed_ * timeStep);

	// TODO distance
	//if (node_->GetWorldPosition2D().Length() > 100.f)
		//node_->Remove();
}
