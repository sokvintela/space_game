#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

static const char* planetTypeNames[] =
{
	"solid_planet",
	"gas_giant",
	0
};

class LPlanet : public LogicComponent
{
	URHO3D_OBJECT(LPlanet, LogicComponent);

public:

	enum PlanetType
	{
		PT_Solid,
		PT_GasGiant,
		PT_Count
	};

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }
	LPlanet(Context* context);

	void Create(PlanetType type, F32 radius, F32 angle);

	PlanetType GetPlanetType() const;
	F32 GetEnergyReceptionCapacity() const;

private:
	PlanetType planetType_;
	F32 energyReceptionCapacity_;
	//Vector<WeakPtr<Planet>> moons_;
};
