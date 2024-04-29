#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "legacyMode/resources/lGameResource.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class LResourceGenerator : public LogicComponent
{
	URHO3D_OBJECT(LResourceGenerator, LogicComponent);

public:

	static void RegisterObject(Context* context);
	LResourceGenerator(Context* context);
	bool SaveDefaultAttributes() const override { return true; }

	bool AddOnGenerate(LGameResource::ResourceType resourceType);

private:
	LGameResource::ResourceType type_;

	virtual void Update(F32 timeStep);
};
