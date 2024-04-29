#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>

#include "tacticalMode/resources/tResourceGenerator.h"
#include "tacticalMode/resources/tResourceReceiver.h"
#include "tacticalMode/resources/tResourceTransmitter.h"
#include "tacticalMode/resources/tGameResource.h"
#include "utility/generalMethods.h"
#include "tacticalMode/gameEvents/tGameEvents.h"

#include "tacticalMode/managers/tResourceManager.h"

String resources("resources");
String energy("energy");
String metals("metals");

String receivers("receivers");
String generators("generators");
String transmitters("transmitters");

using namespace Urho3D;

void TResourceManager::RegisterObject(Context * context)
{
	context->RegisterFactory<TResourceManager>();
}

TResourceManager::TResourceManager(Context* context) :
	LogicComponent(context)
{
}

void TResourceManager::Create() const
{
	Node* resourcesNode = node_->CreateChild(resources);
}

F32 TResourceManager::GetResourceValue(TGameResource::ResourceType type) const
{
	Node* resourcesNode = node_->GetChild(resources);
	if (!resourcesNode)
		return 0.f;

	Node* resourceNode = resourcesNode->GetChild(GetStringByEnum(type));
	if (!resourceNode)
		return 0.f;

	TGameResource* gameResource = resourceNode->GetComponent<TGameResource>();
	if (!gameResource)
		return 0.f;

	return gameResource->GetResourceValue();
}

bool TResourceManager::HasResource(TGameResource::ResourceType type, F32 value) const
{
	Node* resourcesNode = node_->GetChild(resources);
	if (!resourcesNode)
		return false;

	Node* resourceNode = resourcesNode->GetChild(GetStringByEnum(type));
	if (!resourceNode)
		return false;

	TGameResource* gameResource = resourceNode->GetComponent<TGameResource>();
	if (gameResource && (gameResource->GetResourceValue() >= value))
		return true;

	return false;
}

bool TResourceManager::ChangeResourceValue(TGameResource::ResourceType type, F32 value)
{
	if (type == TGameResource::ResourceType::Count)
		return false;

	Node* resourcesNode = node_->GetChild(resources);
	if (!resourcesNode)
		return false;

	Node* resourceNode = resourcesNode->GetChild(GetStringByEnum(type));
	if (!resourceNode)
		return false;

	TGameResource* gameResource = resourceNode->GetComponent<TGameResource>();
	if (!gameResource || !gameResource->ChangeResourceValue(value))
		return false;

	if (type == TGameResource::ResourceType::Metals && GeneralMethods::IsMainPlayer(node_))
	{
		String infoString = "You have got " + String(static_cast<U32>(value)) + " " + tResourceTypeNames[type] + " !";
		SendEvent(RE_INGAME_INFO_STRING, GameInfo::INFO_STRING, infoString);
	}

	return true;
}

bool TResourceManager::SetResourceValue(TGameResource::ResourceType type, F32 value) const
{
	Node* resourcesNode = node_->GetChild(resources);
	if (!resourcesNode)
		return false;

	Node* resourceNode = resourcesNode->GetChild(GetStringByEnum(type));
	if (!resourceNode)
		return false;

	TGameResource* gameResource = resourceNode->GetComponent<TGameResource>();
	if (gameResource && gameResource->SetResourceValue(value))
		return true;

	return false;
}

Vector<TGameResource*> TResourceManager::GetAllResources() const
{
	Vector<TGameResource*> allResources;

	Node* resourcesNode = node_->GetChild(resources);
	if (!resourcesNode)
		return allResources;

	for (U32 i = 0; i < TGameResource::Count; i++)
	{
		TGameResource::ResourceType type = static_cast<TGameResource::ResourceType>(i);
		Node* resourceNode = resourcesNode->GetChild(GetStringByEnum(type));
		if (!resourceNode)
			continue;

		TGameResource* gameResource = resourceNode->GetComponent<TGameResource>();
		if (!gameResource)
			continue;

		allResources.Push(gameResource);
	}

	return allResources;
}

String TResourceManager::GetStringByEnum(TGameResource::ResourceType resourceType) const
{
	switch (resourceType)
	{
	case TGameResource::ResourceType::Energy:
		return energy;
	case TGameResource::ResourceType::Metals:
		return metals;
	}

	return "";
}

String TResourceManager::GetStringByEnum(TGameResource::ResourceDevice deviceType) const
{
	switch (deviceType)
	{
	case TGameResource::ResourceDevice::Generator:
		return generators;
	case TGameResource::ResourceDevice::Receiver:
		return receivers;
	case TGameResource::ResourceDevice::Transmitter:
		return transmitters;
	}

	return "";
}

Node * TResourceManager::GetResourceDeviceNode(TGameResource::ResourceDevice deviceType, TGameResource::ResourceType resourceType) const
{
	String device = GetStringByEnum(deviceType);
	String resource = GetStringByEnum(resourceType);

	Node* resourcesNode = node_->GetChild(resources);
	if (!resourcesNode)
		return nullptr;

	Node* deviceNode = resourcesNode->GetChild(device);
	if (deviceNode)
		return deviceNode->GetChild(resource);

	return nullptr;
}

bool TResourceManager::AddResource(TGameResource::ResourceType resourceType, F32 startValue) const
{
	String resource = GetStringByEnum(resourceType);

	Node* resourcesNode = node_->GetChild(resources);
	if (!resourcesNode)
		return false;

	Node* resourceNode = resourcesNode->GetChild(resource);
	if (resourceNode)
		return false;

	resourceNode = resourcesNode->CreateChild(resource);

	TGameResource* gameResource = resourceNode->CreateComponent<TGameResource>();
	gameResource->AddResource(resourceType, startValue);
	return true;
}

bool TResourceManager::AddResourceDevice(TGameResource::ResourceDevice deviceType, TGameResource::ResourceType resourceType, F32 startValue) const
{
	String device = GetStringByEnum(deviceType);
	String resource = GetStringByEnum(resourceType);

	Node* resourcesNode = node_->GetChild(resources);
	if (!resourcesNode)
		return false;
	
	Node* deviceNode = resourcesNode->GetChild(device);
	if (!deviceNode)
		deviceNode = resourcesNode->CreateChild(device);

	Node* resourceNode = deviceNode->GetChild(resource);
	if (resourceNode)
		return false;

	resourceNode = deviceNode->CreateChild(resource);

	AddResource(resourceType, startValue);
	switch (deviceType)
	{
	case TGameResource::ResourceDevice::Generator:
	{
		TResourceGenerator* generator = resourceNode->CreateComponent<TResourceGenerator>();
		generator->AddOnGenerate(static_cast<TGameResource::ResourceType>(resourceType));
		return true;
	}
	case TGameResource::ResourceDevice::Receiver:
	{
		TResourceReceiver* rResource = resourceNode->CreateComponent<TResourceReceiver>();
		rResource->AddOnReceive(static_cast<TGameResource::ResourceType>(resourceType));
		return true;
	}
	case TGameResource::ResourceDevice::Transmitter:
	{
		TResourceTransmitter* transmitter = resourceNode->CreateComponent<TResourceTransmitter>();
		transmitter->AddOnTransmit(TGameResource::ResourceType::Energy);
		return true;
	}
	}

	return false;
}

bool TResourceManager::HasRecourceDevice(TGameResource::ResourceDevice deviceType, TGameResource::ResourceType resourceType)
{
	String device = GetStringByEnum(deviceType);
	String resource = GetStringByEnum(resourceType);

	Node* resourcesNode = node_->GetChild(resources);
	if (!resourcesNode)
		return false;

	Node* deviceNode = resourcesNode->GetChild(device);
	if (!deviceNode)
		return false;

	Node* resourceNode = deviceNode->GetChild(resource);
	if (!resourceNode)
		return false;

	switch (deviceType)
	{
	case TGameResource::ResourceDevice::Generator:
	{
		return resourceNode->HasComponent<TResourceGenerator>();
	}
	case TGameResource::ResourceDevice::Receiver:
	{
		return resourceNode->HasComponent<TResourceReceiver>();
	}
	case TGameResource::ResourceDevice::Transmitter:
	{
		return resourceNode->HasComponent<TResourceTransmitter>();
	}
	}

	return false;
}
