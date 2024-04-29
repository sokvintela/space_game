#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "legacyMode/resources/lGameResource.h"

using namespace Urho3D;

class LResourceReceiver : public LogicComponent
{
	URHO3D_OBJECT(LResourceReceiver, LogicComponent);

public:

	static void RegisterObject(Context* context);
	LResourceReceiver(Context* context);

	bool AddOnReceive(LGameResource::ResourceType type);

private:
	LGameResource::ResourceType type_;
};
