#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Urho2D/RigidBody2D.h>

#include "equipment/shipEngine.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/spaceship/lSpaceship.h"
#include "stateManager/gameStateEvents.h"
#include "stateManager/statesList.h"
#include "legacyMode/equipment/lEquipment.h"

#include "legacyMode/playerLogic/lControlObject.h"

using namespace Urho3D;

void LControlObject::RegisterObject(Context * context)
{
	context->RegisterFactory<LControlObject>();
}

LControlObject::LControlObject(Context* context) :
	LogicComponent(context)
{
}

void LControlObject::ProcessControls(const Controls& controls)
{
	LEquipmentManager* manager = node_->GetComponent<LEquipmentManager>();
	RigidBody2D* controlRigid = node_->GetComponent<RigidBody2D>();
	if (!manager && !controlRigid)
		return;

	if (controls.buttons_)
	{
		ShipEngine* engine = manager->GetEquipmentItem<ShipEngine>();
		LEquipment* equipment = manager->GetEquipment(Modules::M_EnginePlace);
		if (engine && equipment && equipment->Use())
		{
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
				totalVector += rotation * Vector2::LEFT;
				rollImpulse += rotationSpeed;
			}
			if (controls.buttons_ & CTRL_RIGHT)
			{
				totalVector += rotation * Vector2::RIGHT;
				rollImpulse -= rotationSpeed;
			}

			controlRigid->ApplyForceToCenter(Vector2(totalVector.x_, totalVector.y_) * speed, true);
			controlRigid->ApplyAngularImpulse(rollImpulse, true);
		}
	}

	LSpaceShip* spaceship = node_->GetComponent<LSpaceShip>();
	if (!spaceship)
		return;

	// Process Fire
	if (controls.extraData_.Contains("MousePos"))
	{
		Vector2 mousePos = controls.extraData_["MousePos"]->GetVector2();
		if (controls.buttons_ & MOUSE_LEFT_FIRE)
			spaceship->Shoot(Modules::Module::M_FirstWeaponPlace, mousePos);
		if (controls.buttons_ & MOUSE_RIGHT_FIRE)
			spaceship->Shoot(Modules::Module::M_SecondWeaponPlace, mousePos);
	}
}
