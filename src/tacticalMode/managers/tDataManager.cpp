#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/Localization.h>

#include "utility/generalMethods.h"

#include "tacticalMode/managers/tDataManager.h"

void TDataManager::RegisterObject(Context* context)
{
	context->RegisterFactory<TDataManager>();

	URHO3D_ATTRIBUTE("name", String, name_, "", AM_DEFAULT);

	URHO3D_ATTRIBUTE("clientID", S32, clientID_, -1, AM_DEFAULT);

	URHO3D_ATTRIBUTE("teamNumber", S32, teamNumber_, -1, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("race", race_, Races::raceNames_, Races::Races::R_Random, AM_DEFAULT);

	URHO3D_ATTRIBUTE("efficiency", F32, efficiency_, 0.f, AM_DEFAULT);

	URHO3D_ATTRIBUTE("visibilityArea", F32, visibilityArea_, 0.f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("targetID", S32, targetID_, -1, AM_DEFAULT);
	URHO3D_ATTRIBUTE("breakingDistance", F32, breakingDistance_, 0.f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("breakingForceCoeff", F32, breakingForceCoeff_, 0.f, AM_DEFAULT);
}

TDataManager::TDataManager(Context * context) :
	LogicComponent(context),
	clientID_(-1),
	teamNumber_(-1),
	race_(Races::Races::R_Random),
	efficiency_(0.f),
	visibilityArea_(0.f),
	targetID_(-1),
	breakingDistance_(0.f),
	breakingForceCoeff_(0.f)
{
}

TDataManager::~TDataManager()
{
}

void TDataManager::Create() const
{
}

void TDataManager::SetObjectName(const String& name)
{
	name_ = name;
	MarkNetworkUpdate();
}

String TDataManager::GetObjectName() const
{
	Localization* localization = GetSubsystem<Localization>();
	return localization->Get(name_);
}

void TDataManager::SetClientID(U32 clientID)
{
	clientID_ = clientID;
	node_->SetVar("ClientID", clientID);
	MarkNetworkUpdate();
}

U32 TDataManager::GetClientID() const
{
	return clientID_;
}

S32 TDataManager::GetTeamNumber() const
{
	return teamNumber_;
}

void TDataManager::SetTeamNumber(S32 teamNumber)
{
	teamNumber_ = teamNumber;
	MarkNetworkUpdate();
}

void TDataManager::SetRace(Races::Races race)
{
	race_ = race;
	MarkNetworkUpdate();
}

Races::Races TDataManager::GetRace() const
{
	return race_;
}

bool TDataManager::HasRace() const
{
	if (race_ == Races::Races::R_Random)
		return false;

	return true;
}

F32 TDataManager::GetObjectEfficiency() const
{
	return efficiency_;
}

void TDataManager::SetObjectEfficiency(F32 efficiency)
{
	efficiency_ = efficiency;
	MarkNetworkUpdate();
}

bool TDataManager::HasEfficiency() const
{
	if (efficiency_ == 0.f)
		return false;
	else 
		return true;
}

F32 TDataManager::GetVisibilityArea()
{
	return visibilityArea_;
}

void TDataManager::SetTargetID(S32 targetID)
{
	targetID_ = targetID;
	MarkNetworkUpdate();
}

void TDataManager::SetVisibilityArea(F32 visibilityArea)
{
	visibilityArea_ = visibilityArea;
	MarkNetworkUpdate();
}

F32 TDataManager::GetBreakingForceCoeff()
{
	return breakingForceCoeff_;
}

S32 TDataManager::GetTargetID()
{
	return targetID_;
}
