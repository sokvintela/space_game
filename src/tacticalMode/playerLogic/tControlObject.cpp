#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Urho2D/RigidBody2D.h>

#include "equipment/shipEngine.h"
#include "tacticalMode/managers/tEquipmentManager.h"
#include "tacticalMode/spaceship/tSpaceship.h"
#include "stateManager/gameStateEvents.h"
#include "stateManager/statesList.h"

#include "tacticalMode/playerLogic/tControlObject.h"

using namespace Urho3D;

void TControlObject::RegisterObject(Context * context)
{
	context->RegisterFactory<TControlObject>();
}

TControlObject::TControlObject(Context* context) :
	LogicComponent(context)
{
}

void TControlObject::ProcessControls(const Controls& controls)
{
	TEquipmentManager* manager = node_->GetComponent<TEquipmentManager>();
	RigidBody2D* controlRigid = node_->GetComponent<RigidBody2D>();
	TSpaceShip* spaceship = node_->GetComponent<TSpaceShip>();
	if (!manager && !controlRigid && !spaceship)
		return;

	if (controls.buttons_ && controls.extraData_.Contains("MousePos"))
	{
		ShipEngine* engine = manager->GetElement<ShipEngine>(TEquipmentManager::Module::M_Engine);
		if (engine && manager->IsEquipmentUsed(TEquipmentManager::Module::M_Engine))
		{
			Vector2 mousePos = controls.extraData_["MousePos"]->GetVector2();

			F32 speed = engine->GetSpeed();
			Quaternion rotation = node_->GetRotation();

			F32 rotationSpeed = engine->GetRotationSpeed();
			F32 rollImpulse = 0;

			Vector3 totalVector = Vector3::ZERO;
			if (controls.buttons_ & CTRL_FORWARD)
			{
				totalVector += rotation * Vector2::UP;
			}
			if (controls.buttons_ & CTRL_BACK)
			{
				totalVector += rotation * Vector2::DOWN;
			}
			if (controls.buttons_ & CTRL_LEFT)
			{
				rollImpulse += rotationSpeed;
			}
			if (controls.buttons_ & CTRL_RIGHT)
			{
				rollImpulse -= rotationSpeed;
			}

			if (controls.buttons_ & THIRD_WEAPON_FIRE)
			{
				spaceship->Shoot(TEquipmentManager::Module::M_ThirdWeapon, mousePos);
			}
			if (controls.buttons_ & ULTIMATE_WEAPON_FIRE)
			{
				spaceship->Shoot(TEquipmentManager::Module::M_UltimateWeapon, mousePos);
			}

			controlRigid->ApplyForceToCenter(Vector2(totalVector.x_, totalVector.y_) * speed, true);
			controlRigid->ApplyAngularImpulse(rollImpulse, true);
		}
	}

	// Process Fire
	if (controls.extraData_.Contains("MousePos"))
	{
		Vector2 mousePos = controls.extraData_["MousePos"]->GetVector2();
		if (controls.buttons_ & MOUSE_LEFT_FIRE)
			spaceship->Shoot(TEquipmentManager::Module::M_FirstWeapon, mousePos);
		if (controls.buttons_ & MOUSE_RIGHT_FIRE)
			spaceship->Shoot(TEquipmentManager::Module::M_SecondWeapon, mousePos);
	}
}
