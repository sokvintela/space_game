#pragma once

#include "races/races.h"

enum class CollisionMask
{
	ShipModule =  1,
	Planet =      2,
	Projectile =  4
};

using namespace Urho3D;

namespace Urho3D
{
	class Scene;
}

class SharedData : public Object
{
	URHO3D_OBJECT(SharedData, Object);

public:
	SharedData(Context* context);
	~SharedData();

	void SendGameEvent(StringHash eventType, VariantMap& eventData);
	void SendServerToTeamPlayersEvent(StringHash eventType, U32 teamId, VariantMap& eventData);
	void SendServerToClientsEvent(StringHash eventType, VariantMap& eventData);

	bool isMultiPlayer_;
	bool isLoadGame_;
	bool isConnectToGame_;

	bool isServerWithoutPlayer_;

	/// for client
	WeakPtr<Scene> multiScene_;
	U32 clientId_;

	String loadFileName_;
	String saveFileName_;
	bool inGame_;

	// start parameters
	U32 teamsCount_;

	Races::Races myRace_;
	S32 myTeam_;

	// start player parameters
	String leftWeapon_;
	String rightWeapon_;
	String engine_;
};
