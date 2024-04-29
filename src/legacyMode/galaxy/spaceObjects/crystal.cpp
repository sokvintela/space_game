#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/CollisionCircle2D.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>

#include "legacyMode/galaxy/spaceObjects/lStar.h"
#include "legacyMode/resources/lGameResource.h"
#include "legacyMode/managers/lDataManager.h"

#include "legacyMode/galaxy/spaceObjects/crystal.h"

using namespace Urho3D;

unsigned Crystal::showTimeMs_ = 5000; // 5 seconds

void Crystal::RegisterObject(Context * context)
{
	context->RegisterFactory<Crystal>();

	URHO3D_ATTRIBUTE("type", U32, type_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("especiallity", bool, especiallity_, false, AM_DEFAULT);
}

Crystal::Crystal(Context* context) :
	LogicComponent(context),
	especiallity_(false)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

Crystal::~Crystal()
{
}

bool Crystal::IsEspecial() const
{
	return especiallity_;
}

void Crystal::SetEspeciallity()
{
	especiallity_ = true;
	MarkNetworkUpdate();
}

void Crystal::SetVisible(bool visibility)
{	
	StaticSprite2D* spriteCrystal = node_->GetComponent<StaticSprite2D>();
	if (!spriteCrystal)
		return;

	if (visibility)
	{
		spriteCrystal->SetColor(color_);
		hideTimer_.Reset();
		MarkNetworkUpdate();
	}
	else
		spriteCrystal->SetColor(Color::TRANSPARENT);
}

void Crystal::Create()
{
	RigidBody2D* rigidCrystal = node_->CreateComponent<RigidBody2D>();
	rigidCrystal->SetBodyType(BT_DYNAMIC);
	rigidCrystal->SetAngularDamping(0.0f);

	CollisionCircle2D* circle = node_->CreateComponent<CollisionCircle2D>();
	// TODO: radius
	circle->SetRadius(4.0f);
	circle->SetDensity(1.0f);
	circle->SetFriction(0.5f);
	circle->SetRestitution(0.1f);

	StaticSprite2D* spriteCrystal = node_->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	spriteCrystal->SetSprite(cache->GetResource<Sprite2D>("Urho2D/crystals/cristall.png"));

	type_ = Random(1, LStar::StarType::Count);
	switch (type_)
	{
	case LStar::StarType::OrangeRed:
		spriteCrystal->SetColor(Color::RED);
		break;
	case LStar::StarType::OrangeYellow:
		spriteCrystal->SetColor(Color::MAGENTA);
		break;
	case LStar::StarType::Yellow:
		spriteCrystal->SetColor(Color::YELLOW);
		break;
	case LStar::StarType::YellowWhite:
		spriteCrystal->SetColor(Color::WHITE);
		break;
	case LStar::StarType::White:
		spriteCrystal->SetColor(Color::GRAY);
		break;
	case LStar::StarType::WhiteBlue:
		spriteCrystal->SetColor(Color::BLUE);
		break;
	case LStar::StarType::Blue:
		spriteCrystal->SetColor(Color::CYAN);
		break;
	}

	color_ = spriteCrystal->GetColor();

	F32 coeff = 1.f;
	if (Random(10) < 1)
	{
		especiallity_ = true;
		MarkNetworkUpdate();

		node_->SetScale(15.f);
		// TODO: radius
		circle->SetRadius(4.0f);
		coeff = 1.5f;
	}

	F32 value = coeff * Random(0.f, 400.f);
	node_->SetScale2D(value * node_->GetScale2D());

	LDataManager* dManager = node_->CreateComponent<LDataManager>();
	dManager->Create();
	dManager->SetMyObjectName("Crystal");
	dManager->SetObjectEfficiency(value);

	LGameResource* energy = node_->CreateComponent<LGameResource>();
	energy->AddResource(LGameResource::ResourceType::Energy, value);

	SetVisible(false);
}

void Crystal::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	if (hideTimer_.GetMSec(false) > showTimeMs_)
		SetVisible(false);
}
