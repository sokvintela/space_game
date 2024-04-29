#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class Owner : public LogicComponent
{
	URHO3D_OBJECT(Owner, LogicComponent);

public:
	static void RegisterObject(Context* context);
	Owner(Context* context);
	virtual ~Owner();

	U32 GetOwnerID() const;
	void SetOwnerID(U32 id);

	S32 GetTeamNumber() const;
	void SetTeamNumber(U32 number);

private:
	S32 ownerID_;
	S32 teamNumber_;
};
