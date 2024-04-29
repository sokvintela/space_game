#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "races/races.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class LTeam : public LogicComponent
{
	URHO3D_OBJECT(LTeam, LogicComponent);

public:
	static const U32 NoTeamId = 0xffffffff;

	static void RegisterObject(Context* context);

	LTeam(Context* context);
	~LTeam();

	void Create(U32 teamNumber, Races::Races race);

	// probes
	Node* GetProbesNode() const;
	void AddProbe();

	// star systems
	void SetSurveyedStarSystemsAttr(const PODVector<U8>& value);
	PODVector<U8> GetSurveyedStarSystemsAttr() const;

	void SetFoundStarSystemsAttr(const PODVector<U8>& value);
	PODVector<U8> GetFoundStarSystemsAttr() const;

	bool IsStarSystemFound(U32 id) const;
	bool IsStarSystemSurveyed(U32 id) const;

	void AddFoundStarSystem(U32 id);
	void AddSurveyedStarSystem(U32 id);

	bool IsPlanetSurveyed(U32 starSystemID, U32 planetID);
	struct StarSystemInfo
	{
		HashSet<U32> surveyedPlanets_;
	};

	U32 GetTeamNumber() const;

private:
	U32 numberOfPlayers_;
	HashSet<U16> startingRaces_;

	HashSet<U32> foundStarSystems_;
	HashMap<U32, StarSystemInfo> surveyedStarSystems_;

	U32 number_;
	Races::Races race_;
};
