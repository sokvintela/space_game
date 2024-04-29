#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Network/NetworkEvents.h>

#include "tacticalMode/gameEvents/tGameEvents.h"
#include "stateManager/playersManager.h"

using namespace Urho3D;

U32 PlayersManager::NextPlayerId_ = 0;
static const U32 DEFAULT_TEAM = 0;

PlayersManager::PlayersManager(Urho3D::Context* context)
	: Urho3D::Object(context)
	, allowConnection_(true)
{
	SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(PlayersManager, HandleClientConnected));
	SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(PlayersManager, HandleClientDisconnected));

	SubscribeToEvent(E_NETWORKUPDATE, URHO3D_HANDLER(PlayersManager, HandleNetworkUpdate));
}

PlayersManager::~PlayersManager()
{
	for (auto player : connectedPlayers_)
	{
		if (player.second_.connection)
		{
			player.second_.connection->Disconnect();
		}
	}
}

void PlayersManager::CreateServerSidePlayer()
{
	connectedPlayers_[NextPlayerId_] = PlayersManager::ConnectedPlayer{ NextPlayerId_, DEFAULT_TEAM, nullptr };

	VariantMap newPlayerData;
	newPlayerData[GalaxyNewPlayer::CLIENT_ID] = NextPlayerId_;
	newPlayerData[GalaxyNewPlayer::TEAM_NUMBER] = DEFAULT_TEAM;
	newPlayerData[GalaxyNewPlayer::IS_YOU] = true;

	SendEvent(RE_NEW_PLAYER, newPlayerData);

	NextPlayerId_++;
}

void PlayersManager::SetAllowConnection(bool isAllow)
{
	allowConnection_ = isAllow;
}

void PlayersManager::ProvideScene(Urho3D::Scene* scene)
{
	for (auto& connPlayer : connectedPlayers_)
	{
		if (connPlayer.second_.connection)
		{
			connPlayer.second_.connection->SetScene(scene);
		}
	}
}

void PlayersManager::HandleClientConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
	Connection* newConnection = static_cast<Connection*>(eventData[ClientConnected::P_CONNECTION].GetPtr());
	if (!allowConnection_)
	{
		newConnection->Disconnect();
		return;
	}

	connectedPlayers_[NextPlayerId_] = PlayersManager::ConnectedPlayer{ NextPlayerId_, DEFAULT_TEAM, SharedPtr<Connection>(newConnection)};

	VariantMap newPlayerData;
	newPlayerData[GalaxyNewPlayer::CLIENT_ID] = NextPlayerId_;
	newPlayerData[GalaxyNewPlayer::TEAM_NUMBER] = DEFAULT_TEAM;
	newPlayerData[GalaxyNewPlayer::IS_YOU] = false;

	// inform new client about all others
	for (auto connectedPlayer : connectedPlayers_)
	{
		bool isThisNewPlayer = connectedPlayer.second_.clientId == NextPlayerId_;

		VariantMap connectedPlayerData;
		connectedPlayerData[GalaxyNewPlayer::CLIENT_ID] = connectedPlayer.second_.clientId;
		connectedPlayerData[GalaxyNewPlayer::TEAM_NUMBER] = connectedPlayer.second_.teamId;
		connectedPlayerData[GalaxyNewPlayer::IS_YOU] = isThisNewPlayer;

		newConnection->SendRemoteEvent(RE_NEW_PLAYER, true, connectedPlayerData);

		// inform other clients
		if (!isThisNewPlayer && connectedPlayer.second_.connection)
		{
			connectedPlayer.second_.connection->SendRemoteEvent(RE_NEW_PLAYER, true, newPlayerData);
		}
	}

	SendEvent(RE_NEW_PLAYER, newPlayerData);

	NextPlayerId_++;
}

void PlayersManager::HandleClientDisconnected(StringHash eventType, VariantMap& eventData)
{
	Connection* conn = static_cast<Connection*>(eventData[ClientDisconnected::P_CONNECTION].GetPtr());

	for (auto connectedPlayer : connectedPlayers_)
	{
		if (connectedPlayer.second_.connection == conn)
		{
			VariantMap removePlayerData;
			removePlayerData[GalaxyRemovePlayer::CLIENT_ID] = connectedPlayer.second_.clientId;
			removePlayerData[GalaxyRemovePlayer::TEAM_NUMBER] = connectedPlayer.second_.teamId;

			Network* network = GetSubsystem<Network>();
			network->BroadcastRemoteEvent(RE_REMOVE_PLAYER, true, removePlayerData);
			SendEvent(RE_REMOVE_PLAYER, removePlayerData);

			connectedPlayers_.Erase(connectedPlayer.second_.clientId);
			break;
		}
	}
}

void PlayersManager::HandleNetworkUpdate(StringHash eventType, VariantMap& eventData)
{
	for (auto connectedPlayerIt = connectedPlayers_.Begin(); connectedPlayerIt != connectedPlayers_.End(); )
	{
		if (connectedPlayerIt->second_.connection && !connectedPlayerIt->second_.connection->IsConnected())
		{
			VariantMap removePlayerData;
			removePlayerData[GalaxyRemovePlayer::CLIENT_ID] = connectedPlayerIt->second_.clientId;
			removePlayerData[GalaxyRemovePlayer::TEAM_NUMBER] = connectedPlayerIt->second_.teamId;

			Network* network = GetSubsystem<Network>();
			network->BroadcastRemoteEvent(RE_REMOVE_PLAYER, true, removePlayerData);
			SendEvent(RE_REMOVE_PLAYER, removePlayerData);

			connectedPlayerIt = connectedPlayers_.Erase(connectedPlayerIt);
		}
		else
		{
			++connectedPlayerIt;
		}
	}
}
