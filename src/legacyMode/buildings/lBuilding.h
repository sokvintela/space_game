#pragma once
#include <Urho3D/Scene/LogicComponent.h>

#include "legacyMode/playerLogic/researchTypes.h"

using namespace Urho3D;

static const char* landingTypeNames[] = {
	"Landed",
	"Orbital",
	0
};

static const char* targetTypeNames[] = {
	"Planet",
	"Star",
	"NeutronStar",
	"BlackHole",
	0
};

class LBuilding : public LogicComponent
{
	URHO3D_OBJECT(LBuilding, LogicComponent);

public:

	enum LandingType
	{
		LT_Landed,
		LT_Orbital,
		LT_Count
	};

	enum TargetType
	{
		TT_Planet,
		TT_Star,
		TT_NeutronStar,
		TT_BlackHole,
		TT_Count,
	};

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }

	LBuilding(Context* context);
	~LBuilding() = default;

	LandingType GetLandingType() const;
	TargetType GetTargetType() const;

	Research::ResearchType GetTechnologyType() const;
	void SetTechnologyType(Research::ResearchType type);

	void Create(String buildingName, TargetType targetType);

	Research::ResearchType technology_;

private:
	LandingType landingType_;
	TargetType targetType_;
};
