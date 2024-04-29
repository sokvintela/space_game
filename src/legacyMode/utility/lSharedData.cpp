#include <Urho3D/Network/Network.h>

#include "stateManager/playersManager.h"
#include "legacyMode/utility/lSharedData.h"
#include "races/races.h"

LSharedData::LSharedData(Context * context) :
	Object(context),
	isMultiPlayer_(false),
	isLoadGame_(false),
	isConnectToGame_(false),
	isServerWithoutPlayer_(false),
	inGame_(false),
	clientId_(0),
	isFastGame_(true),
	teamsCount_(2),
	myRace_(Races::Races::R_Irrs),
	myTeam_(0),
	saveFileName_("")
{
	type_ = Galaxy::Type::Spiral2;
	age_ = Galaxy::Age::Medium;

	numberOfStars_ = 15;
	numberOfEnemies_ = 0;
}

LSharedData::~LSharedData()
{
}

void LSharedData::SendGameEvent(StringHash eventType, VariantMap& eventData)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();

	if (!serverConnection)
		SendEvent(eventType, eventData);
	else
		serverConnection->SendRemoteEvent(eventType, true, eventData);
}

void LSharedData::SendServerToTeamPlayersEvent(StringHash eventType, U32 teamId, VariantMap& eventData)
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

void LSharedData::SendServerToClientsEvent(StringHash eventType, VariantMap& eventData)
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
