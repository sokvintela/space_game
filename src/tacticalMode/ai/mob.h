#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

static const char* mobTypeNames[] = {
	"small",
	"big",
	0
};

class Mob : public LogicComponent
{
	URHO3D_OBJECT(Mob, LogicComponent);

public:

	enum MobType
	{
		MT_Small,
		MT_Big,
		MT_Count
	};

	static void RegisterObject(Context* context);
	Mob(Context* context);

	void Create(U32 mobType, U32 teamNumber);

private:
	U32 mobType_;
};
