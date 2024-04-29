#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "races/races.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

class TDataManager : public LogicComponent
{
	URHO3D_OBJECT(TDataManager, LogicComponent);

public:
	static void RegisterObject(Context* context);

	TDataManager(Context* context);
	~TDataManager();

	void Create() const;

	// name
	String GetObjectName() const;
	void SetObjectName(const String& name);

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
	void SetTargetID(S32 targetID);
	void SetVisibilityArea(F32 visibilityArea);

	// info window
	Vector<String> infoWindow_;

private:
	String name_;

	S32 clientID_;

	S32 teamNumber_;
	Races::Races race_;

	F32 efficiency_;

	// AI
	F32 visibilityArea_;
	S32 targetID_;

	F32 breakingDistance_;
	F32 breakingForceCoeff_;
};
