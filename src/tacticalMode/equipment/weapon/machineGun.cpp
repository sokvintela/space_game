#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Urho2D/RigidBody2D.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include "tacticalMode/playerLogic/tControlObject.h"

#include "tacticalMode/equipment/weapon/machineGun.h"

using namespace Urho3D;

void MachineGun::RegisterObject(Context * context)
{
	context->RegisterFactory<MachineGun>();

	URHO3D_ATTRIBUTE("bulletSpeed", F32, bulletSpeed_, 100, AM_DEFAULT);
}

MachineGun::MachineGun(Context* context)
	: WeaponInterface(context)
	, bulletSpeed_(100.0f)
{
}

MachineGun::~MachineGun()
{
}

F32 MachineGun::GetAttackSpeed() const
{
	return bulletSpeed_;
}

void MachineGun::Shoot(Vector2 position)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	//TODO: move to PrefabsStorage
	XMLFile* bulletFileXML = cache->GetResource<XMLFile>("Prefabs/Weapon/MachineGun/bullet.xml");
	Node* bulletNode = node_->GetScene()->InstantiateXML(bulletFileXML->GetRoot(), Vector3::ZERO, Quaternion::IDENTITY);
	node_->AddChild(bulletNode);

	node_->SetWorldScale(0.15f);

	Vector2 direction = position - node_->GetWorldPosition2D();
	direction.Normalize();
	bulletNode->SetWorldPosition2D(node_->GetWorldPosition2D() + direction * 2.0f);

	RigidBody2D* rigidBullet = bulletNode->GetComponent<RigidBody2D>();
	if (rigidBullet)
		rigidBullet->SetLinearVelocity(direction * bulletSpeed_);

	StaticSprite2D* sprite = bulletNode->GetComponent<StaticSprite2D>();
	if (sprite)
		sprite->SetColor(Color::CYAN);

	if (node_->GetParentComponent<TControlObject>(true))
	{
		Sound* sound = cache->GetResource<Sound>("Sounds/gun_shot.wav");
		SoundSource* soundSource = node_->GetOrCreateComponent<SoundSource>();
		soundSource->SetSoundType("effects");
		soundSource->Play(sound);
	}
}
