#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "legacyMode/resources/lGameResource.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class LResourceManager : public LogicComponent
{
	URHO3D_OBJECT(LResourceManager, LogicComponent);

public:

	static void RegisterObject(Context* context);
	LResourceManager(Context* context);

	void Create() const;

	String GetStringByEnum(LGameResource::ResourceType resourceType) const;
	String GetStringByEnum(LGameResource::ResourceDevice deviceType) const;

	Node* GetResourceDeviceNode(LGameResource::ResourceDevice deviceType, LGameResource::ResourceType resourceType) const;
	bool AddResource(LGameResource::ResourceType resourceType, F32 startValue = 0.f) const;
	bool AddResourceDevice(LGameResource::ResourceDevice deviceType, LGameResource::ResourceType resourceType, F32 startValue = 0.f) const;

	F32 GetResourceValue(LGameResource::ResourceType type) const;
	bool HasResource(LGameResource::ResourceType type, F32 value) const;
	bool ChangeResourceValue(LGameResource::ResourceType type, F32 value);
	bool SetResourceValue(LGameResource::ResourceType type, F32 value) const;
};
