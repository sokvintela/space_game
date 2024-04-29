#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>

#include "tacticalMode/gameEvents/tGameEvents.h"
#include "utility/sharedData.h"

#include "tacticalMode/starSystem/tBaseShip.h"

using namespace Urho3D;

void TBaseShip::RegisterObject(Context* context)
{
	context->RegisterFactory<TBaseShip>();
}

TBaseShip::TBaseShip(Context* context) :
	LogicComponent(context)
{
}

void TBaseShip::Create()
{
	SubscribeToEvent(node_, RE_OBJECT_DESTROYED, URHO3D_HANDLER(TBaseShip, HandleObjectDestroyed));
}

void TBaseShip::HandleObjectDestroyed(StringHash eventType, VariantMap & eventData)
{
	GetSubsystem<SharedData>()->SendServerToClientsEvent(RE_GAME_OVER, eventData);
}
