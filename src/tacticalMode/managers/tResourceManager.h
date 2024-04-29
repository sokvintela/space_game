#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "tacticalMode/resources/tGameResource.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class TResourceManager : public LogicComponent
{
	URHO3D_OBJECT(TResourceManager, LogicComponent);

public:

	static void RegisterObject(Context* context);
	TResourceManager(Context* context);

	void Create() const;

	String GetStringByEnum(TGameResource::ResourceType resourceType) const;
	String GetStringByEnum(TGameResource::ResourceDevice deviceType) const;

	Node* GetResourceDeviceNode(TGameResource::ResourceDevice deviceType, TGameResource::ResourceType resourceType) const;
	bool AddResource(TGameResource::ResourceType resourceType, F32 startValue = 0.f) const;
	bool AddResourceDevice(TGameResource::ResourceDevice deviceType, TGameResource::ResourceType resourceType, F32 startValue = 0.f) const;
	bool HasRecourceDevice(TGameResource::ResourceDevice deviceType, TGameResource::ResourceType resourceType);

	F32 GetResourceValue(TGameResource::ResourceType type) const;
	bool HasResource(TGameResource::ResourceType type, F32 value) const;
	bool ChangeResourceValue(TGameResource::ResourceType type, F32 value);
	bool SetResourceValue(TGameResource::ResourceType type, F32 value) const;

	Vector<TGameResource*> GetAllResources() const;
};
