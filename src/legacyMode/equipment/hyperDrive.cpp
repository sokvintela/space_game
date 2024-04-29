#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "legacyMode/spaceship/lSpaceship.h"
#include "legacyMode/galaxy/lStarSystem.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "tacticalMode/equipment/tEquipment.h"

#include "legacyMode/equipment/hyperDrive.h"

static const F32 mathPi = static_cast<float>(M_PI);

using namespace Urho3D;

void HyperDrive::RegisterObject(Context* context)
{
	context->RegisterFactory<HyperDrive>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, hyperEngineTypeNames,HyperDrive::Type::HE_Sun, AM_DEFAULT);
	URHO3D_ATTRIBUTE("speed", F32, speed_, 100.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("move", bool, move_, false, AM_DEFAULT);
	URHO3D_ATTRIBUTE("starSystemID", S32, starSystemID_, -1, AM_DEFAULT);
	URHO3D_ATTRIBUTE("targetPos", Vector2, targetPos_, Vector2(0, 0), AM_DEFAULT);
	URHO3D_ATTRIBUTE("needDistance", F32, needDistance_, 0, AM_DEFAULT);
}

HyperDrive::HyperDrive(Context* context) :
	LogicComponent(context),
	starSystemID_(-1),
	type_(Type::HE_Sun)
{
	move_ = false;
	speed_ = 100.f;
	
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

U32 HyperDrive::GetEngineType() const
{
	return type_;
}

F32 HyperDrive::GetSpeed() const
{
	return speed_;
}

void HyperDrive::SetSpeed(F32 speed)
{
	speed_ = speed;
	MarkNetworkUpdate();
}

void HyperDrive::TravelToPosition(Vector2 position)
{
	LSpaceShip* ship = node_->GetParentComponent<LSpaceShip>(true);
	TEquipment* equipment = node_->GetComponent<TEquipment>();
	if (!ship || !equipment)
		return;

	Vector2 shipPos = ship->GetNode()->GetWorldPosition2D();
	equipment->SetUseCost((position - shipPos).Length());

	if (!equipment->Use())
		return;

	move_ = true;
	targetPos_ = position;
	MarkNetworkUpdate();
}

void HyperDrive::SetStarSystem(U32 starSystemID)
{
	starSystemID_ = starSystemID;

	LStarSystem* system = LGeneralMethods::GetStarSystem(node_, starSystemID_);
	if (!system)
		return;

	needDistance_ = system->starSystemRadius_;
	MarkNetworkUpdate();
}

void HyperDrive::Update(F32 timeStep)
{
	LSpaceShip* ship = node_->GetParentComponent<LSpaceShip>(true);
	if (!ship)
		return;

	Vector2 shipPos = ship->GetNode()->GetWorldPosition2D();
	Vector2 direction = targetPos_ - shipPos;

	if (move_ && (direction.Length() > needDistance_))
	{
		direction.Normalize();
		direction *= (speed_ * timeStep);
		ship->GetNode()->SetWorldPosition2D(shipPos + direction);
	}
	else
	{
		move_ = false;
		MarkNetworkUpdate();

		if (starSystemID_ != -1)
		{
			starSystemID_ = -1;
			MarkNetworkUpdate();
		}
	}
}
