#include <Urho3D/Core/Context.h>

#include "equipment/energyFieldGenerator.h"

using namespace Urho3D;

void EnergyFieldGenerator::RegisterObject(Context* context)
{
	context->RegisterFactory<EnergyFieldGenerator>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, energyFieldGeneratorTypeNames, Type::FG_Sun, AM_DEFAULT);
	URHO3D_ATTRIBUTE("speed", F32, regenerationSpeed_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("maxValue", F32, maxValue_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("timeWaitAfterDamageMs", F32, timeWaitAfterDamageMs_, 2000.0f, AM_DEFAULT);
}

EnergyFieldGenerator::EnergyFieldGenerator(Context* context) :
	LogicComponent(context),
	type_(Type::FG_Sun),
	maxValue_(0.f),
	regenerationSpeed_(0.f),
	timeWaitAfterDamageMs_(2000.f)
{
}

EnergyFieldGenerator::Type EnergyFieldGenerator::GetGeneratorType() const
{
	return type_;
}

F32 EnergyFieldGenerator::GetMaxValue() const
{
	return maxValue_;
}

F32 EnergyFieldGenerator::GetRegenSpeed() const
{
	return regenerationSpeed_;
}

F32 EnergyFieldGenerator::GetTimeWaitAfterDamageDone() const
{
	return timeWaitAfterDamageMs_;
}
