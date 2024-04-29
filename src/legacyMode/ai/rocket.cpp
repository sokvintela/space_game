#include <Urho3D/Core/Context.h>

#include "legacyMode/ai/rocket.h"

using namespace Urho3D;

void Rocket::RegisterObject(Context* context)
{
	context->RegisterFactory<Rocket>();

	URHO3D_ATTRIBUTE("speed", F32, speed_, 1500, AM_DEFAULT);
}

Rocket::Rocket(Context* context)
	: LogicComponent(context),
	speed_(1500.f)
{
}

F32 Rocket::GetSpeed() const
{
	return speed_;
}
