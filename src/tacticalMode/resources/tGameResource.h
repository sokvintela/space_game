#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

#include <map>

using namespace Urho3D;

static const char* tResourceTypeNames[] =
{
	"energy",
	"metals",
	0
};

class TGameResource : public LogicComponent
{
	URHO3D_OBJECT(TGameResource, LogicComponent);

public:
	enum ResourceType
	{
		Energy,
		Metals,
		Count
	};

	enum ResourceDevice
	{
		Generator,
		Receiver,
		Transmitter
	};

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }
	TGameResource(Context* context);

	String GetResourceName() const;

	F32 GetResourceValue() const;
	bool SetResourceValue(F32 value);
	bool ChangeResourceValue(F32 value);

	bool AddResource(ResourceType type, F32 startValue = 0.f);

private:
	ResourceType type_;
	F32 value_;

	virtual void Update(F32 timeStep);
};
