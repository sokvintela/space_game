#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Scene/Scene.h>

#include "tacticalMode/resources/tResourceReceiver.h"
#include "tacticalMode/managers/tResourceManager.h"
#include "utility/generalMethods.h"
#include "tacticalMode/playerLogic/tTeam.h"
#include "tacticalMode/managers/tResourceManager.h"

#include "tacticalMode/resources/tResourceTransmitter.h"

using namespace Urho3D;

void TResourceTransmitter::RegisterObject(Context * context)
{
	context->RegisterFactory<TResourceTransmitter>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, tResourceTypeNames, TGameResource::ResourceType::Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("transmitInterval", F32, transmitInterval_, 5000.f, AM_DEFAULT);
}

TResourceTransmitter::TResourceTransmitter(Context* context) :
	LogicComponent(context),
	type_(TGameResource::ResourceType::Count),
	transmitInterval_(5000.f)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

bool TResourceTransmitter::AddOnTransmit(TGameResource::ResourceType type)
{
	if (type_ == type)
		return false;

	type_ = type;
	MarkNetworkUpdate();
	return true;
}

void TResourceTransmitter::TransmitResource() const
{
	if (type_ == TGameResource::ResourceType::Count)
		return;

	switch (type_)
	{
		case TGameResource::ResourceType::Energy:
		{
			Vector<TResourceManager*> reseiverManagers;

			TStarSystem* system = GeneralMethods::GetOnlyComponent<TStarSystem>(node_);
			if (!system)
				return;

			HashSet<U32> teamIDs = system->teamIDs_;
			for (U32 teamID : teamIDs)
			{
				TTeam* team = GeneralMethods::GetComponent<TTeam>(node_, teamID);
				if (!team)
					continue;

				TResourceManager* rManager = GeneralMethods::GetComponent<TResourceManager>(node_, team->baseShipID_);
				if (rManager && rManager->HasRecourceDevice(TGameResource::ResourceDevice::Receiver, type_))
					reseiverManagers.Push(rManager);

				HashSet<U32> playerIDs = team->playerIDs_;
				for (U32 playerID : playerIDs)
				{
					TResourceManager* rManager = GeneralMethods::GetComponent<TResourceManager>(node_, playerID);
					if (rManager && rManager->HasRecourceDevice(TGameResource::ResourceDevice::Receiver, type_))
						reseiverManagers.Push(rManager);
				}

				HashSet<U32> mobIDs = team->mobIDs_;
				for (U32 mobID : mobIDs)
				{
					TResourceManager* rManager = GeneralMethods::GetComponent<TResourceManager>(node_, mobID);
					if (rManager && rManager->HasRecourceDevice(TGameResource::ResourceDevice::Receiver, type_))
						reseiverManagers.Push(rManager);
				}
			}

			if (reseiverManagers.Empty())
				return;

			TResourceManager* sourceRManager = node_->GetParentComponent<TResourceManager>(true);
			if (!sourceRManager)
				return;

			for (TResourceManager* rManager : reseiverManagers)
			{
				F32 distance = (rManager->GetNode()->GetWorldPosition2D() - node_->GetWorldPosition2D()).Length();
				// TODO : star system radius
				distance = Max(10000.0f, distance);
				rManager->ChangeResourceValue(type_, sourceRManager->GetResourceValue(type_) / distance);
			}

			sourceRManager->SetResourceValue(type_, 0.f);
		}
	}
}

void TResourceTransmitter::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	//if (transmitTimer_.GetMSec(false) > transmitInterval_)
	//{
	TransmitResource();

		//transmitTimer_.Reset();
		//MarkNetworkUpdate();
	//}
}
