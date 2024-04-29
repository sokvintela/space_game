#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class TDebris : public LogicComponent
{
	URHO3D_OBJECT(TDebris, LogicComponent);

public:

	static void RegisterObject(Context* context);
	TDebris(Context* context);
	~TDebris();

	void Create() const;
};