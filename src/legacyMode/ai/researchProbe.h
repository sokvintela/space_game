#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class ResearchProbe : public LogicComponent
{
	URHO3D_OBJECT(ResearchProbe, LogicComponent);

public:

	static void RegisterObject(Context* context);
	ResearchProbe(Context* context);
	~ResearchProbe();

private:
	virtual void Update(F32 timeStep);
};
