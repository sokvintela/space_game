#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/AnimatedSprite2D.h>
#include <Urho3D/Urho2D/AnimationSet2D.h>

#include "tacticalMode/gameEvents/tGameEvents.h"
#include "tacticalMode/managers/tDataManager.h"
#include "tacticalMode/managers/tEquipmentManager.h"
#include "ai/AIBase.h"
#include "tacticalMode/spaceship/tDurability.h"

#include "tacticalMode/starSystem/tPlanet.h"

void TPlanet::RegisterObject(Context* context)
{
	context->RegisterFactory<TPlanet>();
}

TPlanet::TPlanet(Context * context) :
	LogicComponent(context)
{
}

void TPlanet::Create(F32 radius, F32 angle, S32 teamNumber)
{
	node_->SetPosition2D(radius * cosf(angle), radius * sinf(angle));

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	AnimationSet2D* spriterAnimationSet = cache->GetResource<AnimationSet2D>("Urho2D/earth_anim/earth_anim.scml");
	if (!spriterAnimationSet)
		return;

	AnimatedSprite2D* spriterAnimatedSprite = node_->CreateComponent<AnimatedSprite2D>();
	spriterAnimatedSprite->SetAnimationSet(spriterAnimationSet);
	spriterAnimatedSprite->SetAnimation(spriterAnimationSet->GetAnimation(0));
	spriterAnimatedSprite->SetSpeed(0.05f);

	TDataManager* dManager = node_->GetComponent<TDataManager>();
	if (dManager)
		dManager->SetTeamNumber(teamNumber);

	TEquipmentManager* eManager = node_->CreateComponent<TEquipmentManager>();
	eManager->Create();
	eManager->AddElementToModule(TEquipmentManager::Module::M_FirstWeapon, "MachineGun", true);

	AIBase* aiBase = node_->CreateComponent<AIBase>(LOCAL);
	aiBase->SetBehavior("AI/AIPlanet.xml");
	aiBase->Activate(true);

	SubscribeToEvent(node_, RE_OBJECT_DESTROYED, URHO3D_HANDLER(TPlanet, HandleObjectDestroyed));
}

void TPlanet::HandleObjectDestroyed(StringHash eventType, VariantMap & eventData)
{
	TDurability* durability = node_->GetComponent<TDurability>();
	if (durability)
		node_->RemoveComponent<TDurability>();

	AnimatedSprite2D* animate = node_->GetComponent<AnimatedSprite2D>();
	animate->SetColor(Color::GREEN);

	AIBase* aiBase = node_->GetComponent<AIBase>();
	aiBase->Activate(false);
}
