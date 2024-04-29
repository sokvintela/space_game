#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "tacticalMode/resources/tGameResource.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class TResourceGenerator : public LogicComponent
{
	URHO3D_OBJECT(TResourceGenerator, LogicComponent);

public:

	static void RegisterObject(Context* context);
	TResourceGenerator(Context* context);
	bool SaveDefaultAttributes() const override { return true; }

	bool AddOnGenerate(TGameResource::ResourceType resourceType);

private:
	TGameResource::ResourceType type_;

	virtual void Update(F32 timeStep);
};
