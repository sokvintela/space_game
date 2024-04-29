#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "tacticalMode/resources/tGameResource.h"

using namespace Urho3D;

class TResourceReceiver : public LogicComponent
{
	URHO3D_OBJECT(TResourceReceiver, LogicComponent);

public:

	static void RegisterObject(Context* context);
	TResourceReceiver(Context* context);

	bool AddOnReceive(TGameResource::ResourceType type);

private:
	TGameResource::ResourceType type_;
};
