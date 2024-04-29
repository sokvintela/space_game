#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "races/races.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class TTeam : public LogicComponent
{
	URHO3D_OBJECT(TTeam, LogicComponent);

public:
	static const U32 NoTeamId = 0xffffffff;

	U32 baseShipID_;
	HashSet<U32> playerIDs_;
	HashSet<U32> mobIDs_;

	static void RegisterObject(Context* context);
	TTeam(Context* context);
	~TTeam();

	U32 GetTeamNumber() const;

	void Create(U32 teamNumber, Races::Races race);

	void CreateBaseShip();
	void CreatePlayers(U32 numberAi);
	void CreatePlayer(U32 playerNumber, bool isAI = false);

	// for transferring data
	void SetPlayerIdsAttr(const PODVector<U8>& value);
	PODVector<U8> GetPlayerIdsAttr() const;

	void SetMobIdsAttr(const PODVector<U8>& value);
	PODVector<U8> GetMobIdsAttr() const;

private:
	U32 number_;
	Races::Races race_;

	U32 numberOfPlayers_;
	HashSet<U16> startingRaces_;

	Timer mobCreationTimer_;

	virtual void Update(F32 timestep);
};
