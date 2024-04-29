#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>

#include "legacyMode/resources/lGameResource.h"

using namespace Urho3D;

class LResourceTransmitter : public LogicComponent
{
	URHO3D_OBJECT(LResourceTransmitter, LogicComponent);

public:

	static void RegisterObject(Context* context);
	LResourceTransmitter(Context* context);

	bool AddOnTransmit(LGameResource::ResourceType type);
	void TransmitResource(LGameResource::ResourceType type) const;

private:
	LGameResource::ResourceType type_;

	F32 transmitInterval_;
	Timer transmitTimer_;

	virtual void Update(F32 timeStep);
};
