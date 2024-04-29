#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/Sprite2D.h>

#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/resources/tResourceReceiver.h"
#include "tacticalMode/managers/tDataManager.h"
#include "tacticalMode/starSystem/gravity.h"

#include "tacticalMode/starSystem/tStar.h"

void TStar::RegisterObject(Context* context)
{
	context->RegisterFactory<TStar>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, tStarTypeNames, Count, AM_DEFAULT);
}

TStar::TStar(Context * context) :
	LogicComponent(context),
	type_(StarType::OrangeRed)
{
}

void TStar::Create()
{
	RigidBody2D* mRigid = node_->CreateComponent<RigidBody2D>();
	mRigid->SetBodyType(BT_STATIC);
	mRigid->SetAngularDamping(100000.0f);
	mRigid->SetLinearDamping(100000.0f);

	CollisionCircle2D* colCircle = node_->CreateComponent<CollisionCircle2D>();
	// TODO: radius
	colCircle->SetRadius(1.39f);
	colCircle->SetDensity(1.0f);
	colCircle->SetFriction(0.5f);
	colCircle->SetRestitution(0.1f);

	node_->CreateComponent<Gravity>();

	TResourceManager* rManager = node_->CreateComponent<TResourceManager>();
	rManager->Create();
	rManager->AddResourceDevice(TGameResource::ResourceDevice::Generator, TGameResource::ResourceType::Energy);

	TDataManager* dManager = node_->CreateComponent<TDataManager>();
	dManager->SetObjectName("Star");
	dManager->SetObjectEfficiency(100.f * (1.f + 1.f * type_));

	type_ = static_cast<TStar::StarType>(Random(0, TStar::StarType::Count));
	MarkNetworkUpdate();

	StaticSprite2D* sprite = node_->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/sources/star.png"));

	switch (type_)
	{
	case TStar::StarType::OrangeRed:
		sprite->SetColor(Color::RED);
		break;
	case TStar::StarType::OrangeYellow:
		sprite->SetColor(Color::MAGENTA);
		break;
	case TStar::StarType::Yellow:
		sprite->SetColor(Color::YELLOW);
		break;
	case TStar::StarType::YellowWhite:
		sprite->SetColor(Color::WHITE);
		break;
	case TStar::StarType::White:
		sprite->SetColor(Color::WHITE);
		break;
	case TStar::StarType::WhiteBlue:
		sprite->SetColor(Color::BLUE);
		break;
	case TStar::StarType::Blue:
		sprite->SetColor(Color::CYAN);
		break;
	}

	node_->SetScale(40.0f * (1.f + TStar::StarType::Count - static_cast<float>(type_)));
}

TStar::StarType TStar::GetStarType() const
{
	return type_;
}
