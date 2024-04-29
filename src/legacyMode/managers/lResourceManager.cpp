#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>

#include "legacyMode/resources/lResourceGenerator.h"
#include "legacyMode/resources/lResourceReceiver.h"
#include "legacyMode/resources/lResourceTransmitter.h"
#include "legacyMode/resources/lGameResource.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "tacticalMode/gameEvents/tGameEvents.h"

#include "legacyMode/managers/lResourceManager.h"

String lresources("resources");
String lenergy("energy");
String lmetals("metals");
String lknowledge("knowledge");

String lreceivers("receivers");
String lgenerators("generators");
String ltransmitters("transmitters");

using namespace Urho3D;

void LResourceManager::RegisterObject(Context * context)
{
	context->RegisterFactory<LResourceManager>();
}

LResourceManager::LResourceManager(Context* context) :
	LogicComponent(context)
{
}

void LResourceManager::Create() const
{
	Node* resourcesNode = node_->CreateChild(lresources);
}

F32 LResourceManager::GetResourceValue(LGameResource::ResourceType type) const
{
	Node* resourcesNode = node_->GetChild(lresources);
	if (!resourcesNode)
		return 0.f;

	Node* resourceNode = resourcesNode->GetChild(GetStringByEnum(type));
	if (!resourceNode)
		return 0.f;

	LGameResource* gameResource = resourceNode->GetComponent<LGameResource>();
	if (!gameResource)
		return 0.f;

	return gameResource->GetResourceValue();
}

bool LResourceManager::HasResource(LGameResource::ResourceType type, F32 value) const
{
	Node* resourcesNode = node_->GetChild(lresources);
	if (!resourcesNode)
		return false;

	Node* resourceNode = resourcesNode->GetChild(GetStringByEnum(type));
	if (!resourceNode)
		return false;

	LGameResource* gameResource = resourceNode->GetComponent<LGameResource>();
	if (gameResource && (gameResource->GetResourceValue() >= value))
		return true;

	return false;
}

bool LResourceManager::ChangeResourceValue(LGameResource::ResourceType type, F32 value)
{
	if (type == LGameResource::ResourceType::Count)
		return false;

	Node* resourcesNode = node_->GetChild(lresources);
	if (!resourcesNode)
		return false;

	Node* resourceNode = resourcesNode->GetChild(GetStringByEnum(type));
	if (!resourceNode)
		return false;

	LGameResource* gameResource = resourceNode->GetComponent<LGameResource>();
	if (!gameResource || !gameResource->ChangeResourceValue(value))
		return false;

	if (type == LGameResource::ResourceType::Metals && LGeneralMethods::IsMainPlayer(node_))
	{
		String infoString = "You have got " + String(static_cast<U32>(value)) + " " + lResourceTypeNames[type] + " !";
		SendEvent(RE_INGAME_INFO_STRING, GameInfo::INFO_STRING, infoString);
	}

	return true;
}

bool LResourceManager::SetResourceValue(LGameResource::ResourceType type, F32 value) const
{
	Node* resourcesNode = node_->GetChild(lresources);
	if (!resourcesNode)
		return false;

	Node* resourceNode = resourcesNode->GetChild(GetStringByEnum(type));
	if (!resourceNode)
		return false;

	LGameResource* gameResource = resourceNode->GetComponent<LGameResource>();
	if (gameResource && gameResource->SetResourceValue(value))
		return true;

	return false;
}

String LResourceManager::GetStringByEnum(LGameResource::ResourceType resourceType) const
{
	switch (resourceType)
	{
	case LGameResource::ResourceType::Energy:
		return lenergy;
	case LGameResource::ResourceType::Metals:
		return lmetals;
	case LGameResource::ResourceType::Knowledge:
		return lknowledge;
	}

	return "";
}

String LResourceManager::GetStringByEnum(LGameResource::ResourceDevice deviceType) const
{
	switch (deviceType)
	{
	case LGameResource::ResourceDevice::Generator:
		return lgenerators;
	case LGameResource::ResourceDevice::Receiver:
		return lreceivers;
	case LGameResource::ResourceDevice::Transmitter:
		return ltransmitters;
	}

	return "";
}

Node * LResourceManager::GetResourceDeviceNode(LGameResource::ResourceDevice deviceType, LGameResource::ResourceType resourceType) const
{
	String device = GetStringByEnum(deviceType);
	String resource = GetStringByEnum(resourceType);

	Node* resourcesNode = node_->GetChild(lresources);
	if (!resourcesNode)
		return nullptr;

	Node* deviceNode = resourcesNode->GetChild(device);
	if (!deviceNode)
		return nullptr;

	return deviceNode->GetChild(resource);
}

bool LResourceManager::AddResource(LGameResource::ResourceType resourceType, F32 startValue) const
{
	String resource = GetStringByEnum(resourceType);

	Node* resourcesNode = node_->GetChild(lresources);
	if (!resourcesNode)
		return false;

	Node* resourceNode = resourcesNode->GetChild(resource);
	if (resourceNode)
		return false;

	resourceNode = resourcesNode->CreateChild(resource);

	LGameResource* gameResource = resourceNode->CreateComponent<LGameResource>();
	gameResource->AddResource(resourceType, startValue);
	return true;
}

bool LResourceManager::AddResourceDevice(LGameResource::ResourceDevice deviceType, LGameResource::ResourceType resourceType, F32 startValue) const
{
	String device = GetStringByEnum(deviceType);
	String resource = GetStringByEnum(resourceType);

	Node* resourcesNode = node_->GetChild(lresources);
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
	case LGameResource::ResourceDevice::Generator:
	{
		LResourceGenerator* generator = resourceNode->CreateComponent<LResourceGenerator>();
		generator->AddOnGenerate(static_cast<LGameResource::ResourceType>(resourceType));
		return true;
	}
	case LGameResource::ResourceDevice::Receiver:
	{
		LResourceReceiver* rResource = resourceNode->CreateComponent<LResourceReceiver>();
		rResource->AddOnReceive(static_cast<LGameResource::ResourceType>(resourceType));
		return true;
	}
	case LGameResource::ResourceDevice::Transmitter:
	{
		LResourceTransmitter* transmitter = resourceNode->CreateComponent<LResourceTransmitter>();
		transmitter->AddOnTransmit(LGameResource::ResourceType::Energy);
		return true;
	}
	}

	return false;
}
