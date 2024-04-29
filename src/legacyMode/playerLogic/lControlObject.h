#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

// Control bits we define
static const U32 CTRL_FORWARD     = 0b00000001;
static const U32 CTRL_BACK        = 0b00000010;
static const U32 CTRL_LEFT        = 0b00000100;
static const U32 CTRL_RIGHT       = 0b00001000;

static const U32 MOUSE_LEFT_FIRE  = 0b00010000;
static const U32 MOUSE_RIGHT_FIRE = 0b00100000;

class LControlObject : public LogicComponent
{
	URHO3D_OBJECT(LControlObject, LogicComponent);

public:
	static void RegisterObject(Context* context);
	LControlObject(Context* context);

	void ProcessControls(const Controls& controls);
};
