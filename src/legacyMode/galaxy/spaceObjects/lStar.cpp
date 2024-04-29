#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/Sprite2D.h>

#include "legacyMode/managers/lBuildingsManager.h"
#include "legacyMode/managers/lDataManager.h"
#include "legacyMode/managers/lResourceManager.h"

#include "legacyMode/galaxy/spaceObjects/lStar.h"

void LStar::RegisterObject(Context* context)
{
	context->RegisterFactory<LStar>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, starTypeNames, Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("maxStarScale", Vector2, maxStarScale_, Vector2(2.f, 2.f), AM_DEFAULT);
	//URHO3D_ATTRIBUTE("emission", float, emission_, 0.0f, AM_DEFAULT);
}

LStar::LStar(Context * context) :
	LogicComponent(context),
	type_(StarType::OrangeRed),
	maxStarScale_(2.f, 2.f)
{
}

Vector2 LStar::GetMaxStarScale() const
{
	return maxStarScale_;
}

void LStar::Create(StarType type)
{
	LDataManager* dManager = node_->CreateComponent<LDataManager>();
	dManager->Create();
	dManager->SetMyObjectName(starTypeNames[type_] + String(" Star"));
	dManager->SetObjectEfficiency(10.f * (1.f + 1.f * type));

	LResourceManager* rManager = node_->CreateComponent<LResourceManager>();
	rManager->Create();

	LBuildingsManager* bManager = node_->CreateComponent<LBuildingsManager>();
	bManager->Create();

	StaticSprite2D* sprite = node_->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/sources/star.png"));

	if (type == StarType::NeutronStar)
	{
		rManager->AddResourceDevice(LGameResource::ResourceDevice::Generator, LGameResource::ResourceType::Knowledge);

		sprite->SetColor(Color::WHITE);

		node_->SetScale2D(GetMaxStarScale() / 6.f);
	}
	else
	{
		rManager->AddResourceDevice(LGameResource::ResourceDevice::Generator, LGameResource::ResourceType::Energy);

		LStar::StarType type = static_cast<LStar::StarType>(Random(1, LStar::StarType::Count));
		SetStarType(type);
		switch (type)
		{
			case LStar::StarType::OrangeRed:
			{
				sprite->SetColor(Color::RED);
				break;
			}
			case LStar::StarType::OrangeYellow:
			{
				sprite->SetColor(Color::MAGENTA);
				break;
			}
			case LStar::StarType::Yellow:
			{
				sprite->SetColor(Color::YELLOW);
				break;
			}
			case LStar::StarType::YellowWhite:
			{
				sprite->SetColor(Color::WHITE);
				break;
			}
			case LStar::StarType::White:
			{
				sprite->SetColor(Color::WHITE);
				break;
			}
			case LStar::StarType::WhiteBlue:
			{
				sprite->SetColor(Color::BLUE);
				break;
			}
			case LStar::StarType::Blue:
			{
				sprite->SetColor(Color::CYAN);
				break;
			}
		}

		Vector2 addition = -Vector2(1.f, 1.f);
		node_->SetScale2D(5 * (GetMaxStarScale() + addition * static_cast<float>(type)));
	}
}

LStar::StarType LStar::GetStarType() const
{
	return type_;
}

void LStar::SetStarType(StarType type)
{
	type_ = type;
	MarkNetworkUpdate();
}

bool LStar::IsEnergyStar() const
{
	if (type_ == StarType::NeutronStar)
		return false;

	return true;
}
