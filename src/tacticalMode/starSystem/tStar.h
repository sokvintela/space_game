#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

static const char* tStarTypeNames[] =
{
	"orange_red",
	"orange_yellow",
	"yellow",
	"yellow_white",
	"white",
	"white_blue",
	"blue",
	0
};

class TStar : public LogicComponent
{
	URHO3D_OBJECT(TStar, LogicComponent);

public:

	enum StarType
	{
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
	TStar(Context* context);

	Vector2 GetMaxStarScale() const;
	StarType GetStarType() const;

	void Create();

private:
	StarType type_;
};
