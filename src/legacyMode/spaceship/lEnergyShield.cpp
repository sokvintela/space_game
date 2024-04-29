#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "legacyMode/managers/lResourceManager.h"
#include "equipment/energyFieldGenerator.h"
#include "legacyMode/managers/lEquipmentManager.h"

#include "legacyMode/spaceship/lEnergyShield.h"

using namespace Urho3D;

void LEnergyShield::RegisterObject(Context * context)
{
	context->RegisterFactory<LEnergyShield>();

	URHO3D_ATTRIBUTE("value", F32, value_, 0.f, AM_DEFAULT);
}

LEnergyShield::LEnergyShield(Context* context) :
	LogicComponent(context),
	value_(0.f)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

F32 LEnergyShield::GetValue() const
{
	return value_;
}

F32 LEnergyShield::ReceiveDamage(F32 damage)
{
	waitTimer_.Reset();

	value_ -= damage;

	if (value_ >= 0)
		return 0.f;

	F32 damagePassedShield = -value_;

	value_ = 0;
	MarkNetworkUpdate();

	return damagePassedShield;
}

void LEnergyShield::Regenerate(F32 value)
{
	value_ += value;
	MarkNetworkUpdate();
}

void LEnergyShield::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	LEquipmentManager* manager = node_->GetComponent<LEquipmentManager>();
	if (!manager)
		return;

	EnergyFieldGenerator* generator = manager->GetEquipmentItem<EnergyFieldGenerator>();
	if (!generator)
	{
		value_ = 0.f;
		return;
	}

	if (waitTimer_.GetMSec(false) < generator->GetTimeWaitAfterDamageDone())
		return;

	F32 maxValue = generator->GetMaxValue();
	if (value_ >= maxValue)
	{
		value_ = maxValue;
		return;
	}

	LResourceManager* rManager = node_->GetComponent<LResourceManager>();
	if (rManager && rManager->ChangeResourceValue(LGameResource::Energy, generator->GetRegenSpeed() / 10.0f * timeStep))
		Regenerate(generator->GetRegenSpeed() * timeStep);
}
