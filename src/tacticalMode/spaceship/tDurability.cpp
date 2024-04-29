#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>

#include "tacticalMode/gameEvents/tGameEvents.h"
#include "tacticalMode/spaceship/tEnergyShield.h"
#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "tacticalMode/starSystem/tStarSystem.h"
#include "tacticalMode/starSystem/tDebris.h"
#include "utility/generalMethods.h"

#include "tacticalMode/spaceship/tDurability.h"

using namespace Urho3D;

static const U32 debrisCount = 3;

void TDurability::RegisterObject(Context* context)
{
	context->RegisterFactory<TDurability>();

	URHO3D_ATTRIBUTE("durability", F32, durability_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("maxDurability", F32, maxDurability_, 0, AM_DEFAULT);
	URHO3D_ATTRIBUTE("isFallOnDebris", bool, isFallOnDebris_, true, AM_DEFAULT);
}

TDurability::TDurability(Context* context)
	: LogicComponent(context)
	, durability_(20.0f)
	, maxDurability_(20.0f)
	, isFallOnDebris_(true)
{
}

F32 TDurability::GetDurability() const
{
	return durability_;
}

F32 TDurability::GetMaxDurability() const
{
	return maxDurability_;
}

void TDurability::SetDurability(F32 durability)
{
	durability_ = durability;
	MarkNetworkUpdate();
}

void TDurability::SetMaxDurability(F32 durability)
{
	maxDurability_ = durability;
	MarkNetworkUpdate();
}

void TDurability::ChangeDurability(F32 value)
{
	durability_ += value;
	if (durability_ > maxDurability_)
		durability_ = maxDurability_;

	MarkNetworkUpdate();
}

void TDurability::TakeDamage(F32 damage)
{
	F32 shipDamage = damage;

	TEnergyShield* shield = node_->GetComponent<TEnergyShield>();
	if (shield)
		shipDamage = shield->ReceiveDamage(damage);

	if (shipDamage > 0.0f)
	{
		durability_ -= shipDamage;
		if (durability_ <= 0.f)
		{
			durability_ = 0.f;

			if (isFallOnDebris_)
			{
				Node* systemNode = GeneralMethods::GetOnlyNode<TStarSystem>(node_);
				if (systemNode)
					for (U32 i = 0; i < debrisCount; i++)
					{
						// TODO : get from prefabs
						ResourceCache* cache = GetSubsystem<ResourceCache>();
						XMLFile* debrisXML = cache->GetResource<XMLFile>("Prefabs/SpaceObjects/Debris.xml");
						Node* debrisNode = node_->GetScene()->InstantiateXML(debrisXML->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
						systemNode->AddChild(debrisNode);

						TDebris* debris = debrisNode->CreateComponent<TDebris>();
						debris->Create();
						debrisNode->SetWorldPosition2D(node_->GetWorldPosition2D() + Vector2(Random(0.f, 15.f), Random(0.f, 15.f)));
					}

				node_->Remove();
			}

			node_->SendEvent(RE_OBJECT_DESTROYED);
		}
	}
}
