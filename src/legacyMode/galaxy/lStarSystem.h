#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>

#include "legacyMode/galaxy/spaceObjects/lStar.h"
#include "legacyMode/galaxy/spaceObjects/lPlanet.h"

using namespace Urho3D;

class Crystal;

// Former Constellation
class LStarSystem : public LogicComponent
{
	URHO3D_OBJECT(LStarSystem, LogicComponent);

public:
	static void RegisterObject(Context* context);
	LStarSystem(Context* context);

	LStar* GetEnergyStar() const;

	void GenerateSystemContent();
	Vector2 GeneratePlayerPosition(S32 playerSize) const;

	bool HasCrystal() const;
	bool IsCrystalReadyToCreate();
	Crystal* CreateCrystal(bool especial = false);

	void AddStar(LStar::StarType type) const;
	void AddPulsar() const;
	void AddPlanet(LPlanet::PlanetType type, F32 radius, F32 angle) const;

	F32 starSystemRadius_;

private:
	Timer crystalGenerationTimer_;
	static U32 crystalGenerationTimeoutMs_;

	Timer createAsteroidTimer_;
	static U32 createAsteroidTimeMS_;

	virtual void Update(F32 timeStep);
};
