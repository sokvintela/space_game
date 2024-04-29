#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>

#include "tacticalMode/gameEvents/tGameEvents.h"
#include "legacyMode/utility/lSharedData.h"

#include "legacyMode/playerLogic/lBaseShip.h"

using namespace Urho3D;

void LBaseShip::RegisterObject(Context* context)
{
	context->RegisterFactory<LBaseShip>();
}

LBaseShip::LBaseShip(Context* context) :
	LogicComponent(context)
{
}

void LBaseShip::Create()
{
	SubscribeToEvent(node_, RE_OBJECT_DESTROYED, URHO3D_HANDLER(LBaseShip, HandleObjectDestroyed));
}

void LBaseShip::HandleObjectDestroyed(StringHash eventType, VariantMap & eventData)
{
	GetSubsystem<LSharedData>()->SendServerToClientsEvent(RE_GAME_OVER, eventData);
}
