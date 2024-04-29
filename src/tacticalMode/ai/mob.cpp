#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include "tacticalMode/managers/tEquipmentManager.h"
#include "tacticalMode/managers/tDataManager.h"
#include "tacticalMode/starSystem/gravityAffected.h"

#include "tacticalMode/ai/mob.h"
#include "ai/AIBase.h"

using namespace Urho3D;

void Mob::RegisterObject(Context* context)
{
	context->RegisterFactory<Mob>();

	URHO3D_ENUM_ATTRIBUTE("mobType_", mobType_, mobTypeNames, MT_Count, AM_DEFAULT);
}

Mob::Mob(Context* context) :
	LogicComponent(context),
	mobType_(MT_Count)
{
}

void Mob::Create(U32 mobType, U32 teamNumber)
{
	mobType_ = mobType;
	MarkNetworkUpdate();

	TEquipmentManager* equipment = node_->CreateComponent<TEquipmentManager>();
	equipment->Create();
	equipment->AddElementToModule(TEquipmentManager::Module::M_Engine, "Chemical");
	equipment->AddElementToModule(TEquipmentManager::Module::M_FirstWeapon, "MachineGun", true);

	TDataManager* dManager = node_->GetComponent<TDataManager>();
	if (dManager)
		dManager->SetTeamNumber(teamNumber);

	AIBase* aiBase = node_->CreateComponent<AIBase>(LOCAL);
	aiBase->SetBehavior("AI/AIMob.xml");
	aiBase->Activate(true);

	//node_->CreateComponent<GravityAffected>();

	StaticSprite2D* sprite = node_->CreateComponent<StaticSprite2D>();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	sprite->SetSprite(cache->GetResource<Sprite2D>("Urho2D/ships/5.png"));
}
