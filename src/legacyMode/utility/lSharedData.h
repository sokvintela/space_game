#pragma once

#include "legacyMode/galaxy/galaxy.h"
#include "races/races.h"

enum class LCollisionMask
{
	ShipModule =  1,
	Planet =      2,
	Projectile =  4
};

using namespace Urho3D;

namespace Urho3D
{
	class Scene;
	class Connection;
}

class LSharedData : public Object
{
	URHO3D_OBJECT(LSharedData, Object);

public:
	LSharedData(Context* context);
	~LSharedData();

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
	bool isFastGame_;
	U32 teamsCount_;

	Races::Races myRace_;
	S32 myTeam_;
	Galaxy::Type type_;
	Galaxy::Age age_;
	S32 numberOfStars_;
	S32 numberOfEnemies_;

	// start player parameters
	String leftWeapon_;
	String rightWeapon_;
	String engine_;
};
