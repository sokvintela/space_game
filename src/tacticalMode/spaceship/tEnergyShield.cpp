#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "tacticalMode/managers/tResourceManager.h"
#include "equipment/energyFieldGenerator.h"
#include "tacticalMode/managers/tEquipmentManager.h"

#include "tacticalMode/spaceship/tEnergyShield.h"

using namespace Urho3D;

void TEnergyShield::RegisterObject(Context * context)
{
	context->RegisterFactory<TEnergyShield>();

	URHO3D_ATTRIBUTE("value", F32, value_, 0.f, AM_DEFAULT);
}

TEnergyShield::TEnergyShield(Context* context) :
	LogicComponent(context),
	value_(0.f)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

F32 TEnergyShield::GetValue() const
{
	return value_;
}

F32 TEnergyShield::ReceiveDamage(F32 damage)
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

void TEnergyShield::Regenerate(F32 value)
{
	value_ += value;
	MarkNetworkUpdate();
}

void TEnergyShield::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	EnergyFieldGenerator* generator = node_->GetComponent<EnergyFieldGenerator>(true);
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

	TResourceManager* rManager = node_->GetComponent<TResourceManager>();
	if (rManager && rManager->ChangeResourceValue(TGameResource::Energy, generator->GetRegenSpeed() / 10.0f * timeStep))
		Regenerate(generator->GetRegenSpeed() * timeStep);
}
