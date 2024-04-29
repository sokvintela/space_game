#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

static const char* starTypeNames[] =
{
	"neutron",
	"orange_red",
	"orange_yellow",
	"yellow",
	"yellow_white",
	"white",
	"white_blue",
	"blue",
	0
};

class LStar : public LogicComponent
{
	URHO3D_OBJECT(LStar, LogicComponent);

public:

	enum StarType
	{
		NeutronStar,
		OrangeRed,
		OrangeYellow,
		Yellow,
		YellowWhite,
		White,
		WhiteBlue,
		Blue,
		Count
	};

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }
	LStar(Context* context);

	void Create(StarType type);

	Vector2 GetMaxStarScale() const;

	StarType GetStarType() const;
	void SetStarType(StarType type);
	bool IsEnergyStar() const;

private:
	StarType type_;
	Vector2 maxStarScale_;
	//F32 emission_;
};
