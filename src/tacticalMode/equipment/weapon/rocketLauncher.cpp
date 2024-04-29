#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>

#include <Urho3D/Graphics/DebugRenderer.h>

#include "tacticalMode/playerLogic/tControlObject.h"
#include "tacticalMode/spaceship/tDurability.h"

#include "utility/selfDestroyingTimer.h"

#include "tacticalMode/equipment/weapon/weapon.h"
#include "tacticalMode/equipment/weapon/rocketLauncher.h"

using namespace Urho3D;

void RocketLauncher::RegisterObject(Context * context)
{
	URHO3D_ATTRIBUTE("rocketSpeed", F32, rocketSpeed_, 100, AM_DEFAULT);

	context->RegisterFactory<RocketLauncher>();
}

RocketLauncher::RocketLauncher(Context* context)
	: WeaponInterface(context)
{
}

RocketLauncher::~RocketLauncher()
{
}

F32 RocketLauncher::GetAttackSpeed() const
{
	return rocketSpeed_;
}

void RocketLauncher::Shoot(Vector2 position)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	Node* rocketNode = node_->CreateChild("Rocket");

	SelfDestroyingTimer* timer = rocketNode->CreateComponent<SelfDestroyingTimer>();
	timer->SetLifeTime(10000.0f);

	StaticSprite2D* staticSprite = rocketNode->CreateComponent<StaticSprite2D>();
	Sprite2D* sprite = cache->GetResource<Sprite2D>("Urho2D/shells/annihilation.png");
	staticSprite->SetSprite(sprite);

	Vector2 direction = position - node_->GetWorldPosition2D();
	direction.Normalize();
	rocketNode->SetWorldPosition2D(node_->GetWorldPosition2D() + direction * 2.0f);

	RigidBody2D* rigidBullet = rocketNode->GetComponent<RigidBody2D>();
	if (rigidBullet)
		rigidBullet->SetLinearVelocity(direction * rocketSpeed_);

}
