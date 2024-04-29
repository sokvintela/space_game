#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "tacticalMode/starSystem/gravityAffected.h"

using namespace Urho3D;

void GravityAffected::RegisterObject(Context * context)
{
	context->RegisterFactory<GravityAffected>();
}

GravityAffected::GravityAffected(Context* context) :
	LogicComponent(context)
{
}

GravityAffected::~GravityAffected()
{
}
