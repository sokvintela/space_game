#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

static const char* energyFieldGeneratorTypeNames[] =
{
	"sun",
	"chemical",
	"ion",
	"plasma",
	"nuclear",
	"photon",
	0
};

class EnergyFieldGenerator : public LogicComponent
{
	URHO3D_OBJECT(EnergyFieldGenerator, LogicComponent);

public:
	enum Type
	{
		FG_Sun,
		FG_Chemical,
		FG_Ion,
		FG_Plasma,
		FG_Nuclear,
		FG_Photon,
		FG_Count
	};

	static void RegisterObject(Context* context);
	EnergyFieldGenerator(Context* context);

	Type GetGeneratorType() const;
	F32 GetMaxValue() const;
	F32 GetRegenSpeed() const;
	F32 GetTimeWaitAfterDamageDone() const;

private:
	Type type_;
	F32 maxValue_;
	F32 regenerationSpeed_;

	F32 timeWaitAfterDamageMs_;
};
