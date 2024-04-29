#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "races/races.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class LDataManager : public LogicComponent
{
	URHO3D_OBJECT(LDataManager, LogicComponent);

public:
	static void RegisterObject(Context* context);

	LDataManager(Context* context);
	~LDataManager();

	void Create() const;

	// name
	String GetObjectName(Node* objectNode = nullptr) const;
	void SetMyObjectName(String name);
	void SetEnemyObjectName(String name);

	// client
	U32 GetClientID() const;
	void SetClientID(U32 clientID);

	// team
	S32 GetTeamNumber() const;
	void SetTeamNumber(S32 teamNumber);

	// race
	Races::Races GetRace() const;
	void SetRace(Races::Races race);
	bool HasRace() const;

	// efficiency
	F32 GetObjectEfficiency() const;
	void SetObjectEfficiency(F32 efficiency);
	bool HasEfficiency() const;

	// ai
	F32 GetVisibilityArea();
	F32 GetBreakingForceCoeff();
	S32 GetTargetID();
	void SetTargetID(U32 targetID);

	// star systems
	bool IsInStarSystem() const;

	S32 GetCurrentStarSystemID() const;
	void SetCurrentStarSystemID(U32 systemID);
	void HeadOffStarSystem();

	// info window
	Vector<String> infoWindow_;

private:
	String myName_;
	String enemyName_;

	S32 clientID_;

	S32 teamNumber_;
	Races::Races race_;

	F32 efficiency_;

	// AI
	F32 visibilityArea_;
	S32 targetID_;

	F32 breakingDistance_;
	F32 breakingForceCoeff_;

	// old
	S32 currentStarSystemID_;

	void Update(F32 timeStep);
};
