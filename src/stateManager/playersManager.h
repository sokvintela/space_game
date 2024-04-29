#pragma once

#include <Urho3D/Network/Connection.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/HashMap.h>

#include "utility/simpleTypes.h"

class PlayersManager : public Urho3D::Object
{
	URHO3D_OBJECT(PlayersManager, Urho3D::Object);

public:
	PlayersManager(Urho3D::Context* context);
	~PlayersManager();

	void CreateServerSidePlayer();
	void SetAllowConnection(bool isAllow);
	void ProvideScene(Urho3D::Scene* scene);

	struct ConnectedPlayer
	{
		U32 clientId;
		U32 teamId;

		Urho3D::SharedPtr<Urho3D::Connection> connection;
	};

	// server holds informations about players
	Urho3D::HashMap<U32, ConnectedPlayer> connectedPlayers_;

	static U32 NextPlayerId_;
private:

	bool allowConnection_;

	void HandleClientConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleClientDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleNetworkUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};
