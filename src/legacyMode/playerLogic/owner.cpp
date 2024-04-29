#include <Urho3D/Core/Context.h>

#include "legacyMode/playerLogic/owner.h"

void Owner::RegisterObject(Context* context)
{
	context->RegisterFactory<Owner>();

	URHO3D_ATTRIBUTE("ownerID", S32, ownerID_, -1, AM_DEFAULT);
	URHO3D_ATTRIBUTE("teamNumber", S32, teamNumber_, -1, AM_DEFAULT);
}

Owner::Owner(Context * context) :
	LogicComponent(context),
	ownerID_(-1),
	teamNumber_(-1)
{
}

Owner::~Owner()
{
}

S32 Owner::GetTeamNumber() const
{
	return teamNumber_;
}

void Owner::SetTeamNumber(U32 id)
{
	teamNumber_ = id;
}

U32 Owner::GetOwnerID() const
{
	return ownerID_;
}

void Owner::SetOwnerID(U32 id)
{
	ownerID_ = id;
	MarkNetworkUpdate();
}
