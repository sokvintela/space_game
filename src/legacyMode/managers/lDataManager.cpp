#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "legacyMode/utility/lGeneralMethods.h"

#include "legacyMode/managers/lDataManager.h"

void LDataManager::RegisterObject(Context* context)
{
	context->RegisterFactory<LDataManager>();

	URHO3D_ATTRIBUTE("myName", String, myName_, "", AM_DEFAULT);
	URHO3D_ATTRIBUTE("enemyName", String, enemyName_, "", AM_DEFAULT);

	URHO3D_ATTRIBUTE("clientID", S32, clientID_, -1, AM_DEFAULT);

	URHO3D_ATTRIBUTE("teamNumber", S32, teamNumber_, -1, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("race", race_, Races::raceNames_, Races::Races::R_Random, AM_DEFAULT);

	URHO3D_ATTRIBUTE("efficiency", F32, efficiency_, 0.f, AM_DEFAULT);

	URHO3D_ATTRIBUTE("visibilityArea", F32, visibilityArea_, 0.f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("targetID", S32, targetID_, -1, AM_DEFAULT);
	URHO3D_ATTRIBUTE("breakingDistance", F32, breakingDistance_, 0.f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("breakingForceCoeff", F32, breakingForceCoeff_, 0.f, AM_DEFAULT);

	URHO3D_ATTRIBUTE("currentStarSystemID", S32, currentStarSystemID_, -1, AM_DEFAULT);
}

LDataManager::LDataManager(Context * context) :
	LogicComponent(context),
	clientID_(-1),
	currentStarSystemID_(-1),
	teamNumber_(-1),
	race_(Races::Races::R_Random),
	myName_(""),
	enemyName_(""),
	efficiency_(0.f),
	visibilityArea_(0.f),
	targetID_(-1),
	breakingDistance_(0.f),
	breakingForceCoeff_(0.f)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

LDataManager::~LDataManager()
{
}

void LDataManager::Create() const
{
}

void LDataManager::SetMyObjectName(String name)
{
	myName_ = name;
	MarkNetworkUpdate();
}

void LDataManager::SetEnemyObjectName(String name)
{
	enemyName_ = name;
	MarkNetworkUpdate();
}

String LDataManager::GetObjectName(Node* objectNode) const
{
	if (!objectNode)
		return myName_;
	else
		return enemyName_;

	return "";
}

void LDataManager::SetClientID(U32 clientID)
{
	clientID_ = clientID;
	node_->SetVar("ClientID", clientID);
	MarkNetworkUpdate();
}

U32 LDataManager::GetClientID() const
{
	return clientID_;
}

S32 LDataManager::GetTeamNumber() const
{
	return teamNumber_;
}

void LDataManager::SetTeamNumber(S32 teamNumber)
{
	teamNumber_ = teamNumber;
	MarkNetworkUpdate();
}

S32 LDataManager::GetCurrentStarSystemID() const
{
	return currentStarSystemID_;
}

bool LDataManager::IsInStarSystem() const
{
	if (currentStarSystemID_ == -1)
		return false;

	return true;
}

void LDataManager::HeadOffStarSystem()
{
	currentStarSystemID_ = -1;
	MarkNetworkUpdate();
}

void LDataManager::SetRace(Races::Races race)
{
	race_ = race;
	MarkNetworkUpdate();
}

Races::Races LDataManager::GetRace() const
{
	return race_;
}

bool LDataManager::HasRace() const
{
	if (race_ == Races::Races::R_Random)
		return false;

	return true;
}

F32 LDataManager::GetObjectEfficiency() const
{
	return efficiency_;
}

void LDataManager::SetObjectEfficiency(F32 efficiency)
{
	efficiency_ = efficiency;
	MarkNetworkUpdate();
}

bool LDataManager::HasEfficiency() const
{
	if (efficiency_ == 0.f)
		return false;
	else 
		return true;
}

F32 LDataManager::GetVisibilityArea()
{
	return visibilityArea_;
}

void LDataManager::SetTargetID(U32 targetID)
{
	targetID_ = targetID;
	MarkNetworkUpdate();
}

F32 LDataManager::GetBreakingForceCoeff()
{
	return breakingForceCoeff_;
}

S32 LDataManager::GetTargetID()
{
	return targetID_;
}

void LDataManager::SetCurrentStarSystemID(U32 id)
{
	currentStarSystemID_ = id;
	MarkNetworkUpdate();
}

void LDataManager::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	// current star system
	HeadOffStarSystem();

	PODVector<Node*> starSystemNodes = LGeneralMethods::GetStarSystemNodes(node_);
	for (U32 i = 0; i < starSystemNodes.Size(); i++)
		// TODO: starSystem->starSystemRadius_ too little
		if ((node_->GetWorldPosition2D() - starSystemNodes[i]->GetWorldPosition2D()).Length() < 100.f)
			SetCurrentStarSystemID(starSystemNodes[i]->GetID());
}
