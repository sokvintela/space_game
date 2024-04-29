#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>

#include "tacticalMode/resources/tGameResource.h"

using namespace Urho3D;

class TResourceTransmitter : public LogicComponent
{
	URHO3D_OBJECT(TResourceTransmitter, LogicComponent);

public:

	static void RegisterObject(Context* context);
	TResourceTransmitter(Context* context);

	bool AddOnTransmit(TGameResource::ResourceType type);
	void TransmitResource() const;

private:
	TGameResource::ResourceType type_;

	F32 transmitInterval_;
	Timer transmitTimer_;

	virtual void Update(F32 timeStep);
};
