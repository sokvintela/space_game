#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Scene/Scene.h>

#include "legacyMode/playerLogic/owner.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/managers/lDataManager.h"
#include "legacyMode/resources/lResourceReceiver.h"
#include "legacyMode/managers/lResourceManager.h"

#include "legacyMode/resources/lResourceTransmitter.h"

using namespace Urho3D;

void LResourceTransmitter::RegisterObject(Context * context)
{
	context->RegisterFactory<LResourceTransmitter>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, lResourceTypeNames, LGameResource::ResourceType::Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("transmitInterval", F32, transmitInterval_, 5000.f, AM_DEFAULT);
}

LResourceTransmitter::LResourceTransmitter(Context* context) :
	LogicComponent(context),
	type_(LGameResource::ResourceType::Count),
	transmitInterval_(5000.f)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

bool LResourceTransmitter::AddOnTransmit(LGameResource::ResourceType type)
{
	if (type_ == type)
		return false;

	type_ = type;
	MarkNetworkUpdate();
	return true;
}

void LResourceTransmitter::TransmitResource(LGameResource::ResourceType type) const
{
	if (type_ == LGameResource::ResourceType::Count)
		return;

	LDataManager* objectDManager = node_->GetParentComponent<LDataManager>(true);
	if (!objectDManager)
		return;

	int objectSystemID = objectDManager->GetCurrentStarSystemID();
	switch (type)
	{
		case LGameResource::ResourceType::Energy:
		{
			Owner* owner = node_->GetParentComponent<Owner>(true);
			if (owner)
			{
				Node* ownerNode = owner->GetNode();
				LDataManager* ownerDManager = LGeneralMethods::GetOwnerDataManager(ownerNode);
				if (ownerDManager && ownerDManager->GetCurrentStarSystemID() == objectSystemID)
				{
					LResourceManager* shipRManager = ownerNode->GetComponent<LResourceManager>();
					LResourceManager* transmittableRManager = node_->GetParentComponent<LResourceManager>(true);
					if (shipRManager && transmittableRManager)
					{
						shipRManager->ChangeResourceValue(type, transmittableRManager->GetResourceValue(type));
						transmittableRManager->SetResourceValue(type, 0.f);
					}
				}
			}
			else
			{
				Scene* scene = node_->GetScene();
				if (!scene)
					return;

				PODVector<Node*> reseiverNodes;
				scene->GetChildrenWithComponent<LResourceReceiver>(reseiverNodes, true);
				if (reseiverNodes.Empty())
					return;

				LResourceManager* sourceRManager = node_->GetParentComponent<LResourceManager>(true);
				for (auto resourceNodeIt = reseiverNodes.Begin(); resourceNodeIt != reseiverNodes.End(); ++resourceNodeIt)
				{
					auto resourceNode = *resourceNodeIt;

					LDataManager* receivedDManager = resourceNode->GetParentComponent<LDataManager>(true);
					LResourceManager* rManagerToReceive = resourceNode->GetParentComponent<LResourceManager>(true);
					if (!receivedDManager || objectSystemID != receivedDManager->GetCurrentStarSystemID()
						|| !rManagerToReceive || !sourceRManager)
						continue;

					F32 energyReceptionCapacity = 1.f;
					LPlanet* planet = resourceNode->GetParentComponent<LPlanet>(true);
					if (planet)
						energyReceptionCapacity = planet->GetEnergyReceptionCapacity();

					F32 distanceSquared = (resourceNode->GetWorldPosition2D() - node_->GetWorldPosition2D()).Length();
					// TODO : star system radius
					distanceSquared = Max(10.0f, distanceSquared);
					rManagerToReceive->ChangeResourceValue(type, sourceRManager->GetResourceValue(type) * energyReceptionCapacity / distanceSquared);
				}

				sourceRManager->SetResourceValue(type, 0.f);
			}

			break;
		}
		case LGameResource::ResourceType::Knowledge:
		{
			Owner* owner = node_->GetParentComponent<Owner>(true);
			if (!owner)
				return;

			Node* controlNode = LGeneralMethods::GetOwnerNode(owner->GetNode());
			if (!controlNode)
				return;

			LResourceManager* shipRManager = controlNode->GetComponent<LResourceManager>();
			LResourceManager* transmittableRManager = node_->GetParentComponent<LResourceManager>(true);
			if (shipRManager && transmittableRManager)
			{
				shipRManager->ChangeResourceValue(type, transmittableRManager->GetResourceValue(type));
				transmittableRManager->SetResourceValue(type, 0.f);
			}

			break;
		}
		case LGameResource::ResourceType::Metals:
		{
			break;
		}
	}
}

void LResourceTransmitter::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	//if (transmitTimer_.GetMSec(false) > transmitInterval_)
	//{
	TransmitResource(type_);

		//transmitTimer_.Reset();
		//MarkNetworkUpdate();
	//}
}
