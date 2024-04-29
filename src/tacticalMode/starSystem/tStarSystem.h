#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

// Former Constellation
class TStarSystem : public LogicComponent
{
	URHO3D_OBJECT(TStarSystem, LogicComponent);

public:
	U32 starID_;
	HashSet<U32> asteroidIDs_;
	HashSet<U32> planetIDs_;
	HashSet<U32> teamIDs_;

	static void RegisterObject(Context* context);
	TStarSystem(Context* context);

	void GenerateSystemContent();

	void CreateTeams();

	void CreateStar();
	void CreateAsteroidBelt();
	void CreatePlanet(F32 size, F32 radius, F32 angle, U32 teamNumber);

	F32 starSystemRadius_;


	// for transferring data
	void SetAsteroidIdsAttr(const PODVector<U8>& value);
	PODVector<U8> GetAsteroidIdsAttr() const;

	void SetPlanetIdsAttr(const PODVector<U8>& value);
	PODVector<U8> GetPlanetIdsAttr() const;

	void SetTeamIdsAttr(const PODVector<U8>& value);
	PODVector<U8> GetTeamIdsAttr() const;
};
