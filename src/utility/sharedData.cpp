#include <Urho3D/Network/Network.h>

#include "stateManager/playersManager.h"
#include "utility/sharedData.h"

SharedData::SharedData(Context * context) :
	Object(context),
	isMultiPlayer_(false),
	isLoadGame_(false),
	isConnectToGame_(false),
	isServerWithoutPlayer_(false),
	inGame_(false),
	clientId_(0),
	teamsCount_(2),
	myRace_(Races::Races::R_Irrs),
	myTeam_(0),
	saveFileName_("")
{
}

SharedData::~SharedData()
{
}

void SharedData::SendGameEvent(StringHash eventType, VariantMap& eventData)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();

	if (!serverConnection)
		SendEvent(eventType, eventData);
	else
		serverConnection->SendRemoteEvent(eventType, true, eventData);
}

void SharedData::SendServerToTeamPlayersEvent(StringHash eventType, U32 teamId, VariantMap& eventData)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();

	if (serverConnection)
		return;

	PlayersManager* playersManager = GetSubsystem<PlayersManager>();
	if (!playersManager)
		return;

	for (auto connectedPlayer : playersManager->connectedPlayers_)
	{
		if (connectedPlayer.second_.teamId == teamId)
		{
			if (connectedPlayer.second_.connection)
				connectedPlayer.second_.connection->SendRemoteEvent(eventType, true, eventData);
			else
				SendEvent(eventType, eventData);
		}
	}
}

void SharedData::SendServerToClientsEvent(StringHash eventType, VariantMap& eventData)
{
	PlayersManager* playersManager = GetSubsystem<PlayersManager>();
	if (!playersManager)
		return;

	for (auto connectedPlayer : playersManager->connectedPlayers_)
	{
		if (connectedPlayer.second_.connection)
			connectedPlayer.second_.connection->SendRemoteEvent(eventType, true, eventData);
		else
			SendEvent(eventType, eventData);
	}
}
