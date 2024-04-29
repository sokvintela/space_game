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
#include "tacticalMode/equipment/weapon/laser.h"

using namespace Urho3D;

F32 Laser::attackSpeed_ = 10000.0f;

void Laser::RegisterObject(Context * context)
{
	context->RegisterFactory<Laser>();
}

Laser::Laser(Context* context)
	: WeaponInterface(context)
{
}

Laser::~Laser()
{
}

F32 Laser::GetAttackSpeed() const
{
	return Laser::attackSpeed_;
}

void Laser::Shoot(Vector2 position)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	Node* laserRayNode = node_->CreateChild("LaserRay");

	SelfDestroyingTimer* timer = laserRayNode->CreateComponent<SelfDestroyingTimer>();
	timer->SetLifeTime(1000.0f);

	laserRayNode->SetScale2D(100.0f, 10.0f);

	PhysicsWorld2D* world2D = node_->GetScene()->GetComponent<PhysicsWorld2D>();

	PhysicsRaycastResult2D raycastResult;
	world2D->RaycastSingle(raycastResult, node_->GetWorldPosition2D(), position);

	Vector3 endPosition = position;
	if (raycastResult.body_ != nullptr)
	{
		endPosition = raycastResult.position_;

		Weapon* weapon = node_->GetComponent<Weapon>();
		TDurability* durability = raycastResult.body_->GetComponent<TDurability>();
		if (durability && weapon)
			durability->TakeDamage(weapon->GetDamage());
	}

	// TODO: use sprites for laser work, not debug geometry
	// StaticSprite2D* staticSprite = laserRayNode->CreateComponent<StaticSprite2D>();
	// Sprite2D* sprite = cache->GetResource<Sprite2D>("Textures/laser.png");
	// staticSprite->SetSprite(sprite);

	DebugRenderer* debugRenderer = node_->GetScene()->GetComponent<DebugRenderer>();
	debugRenderer->AddLine(node_->GetWorldPosition(), endPosition, Color::RED);
}
