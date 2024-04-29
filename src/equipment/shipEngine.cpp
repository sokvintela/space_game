#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>

#include "equipment/shipEngine.h"

using namespace Urho3D;

void ShipEngine::RegisterObject(Context* context)
{
	context->RegisterFactory<ShipEngine>();

	URHO3D_ENUM_ATTRIBUTE("type", type_, engineTypeNames, ShipEngine::Type::SE_Sun, AM_DEFAULT);
	URHO3D_ATTRIBUTE("speed", F32, speed_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("rotationSpeed", F32, rotationSpeed_, 0.0f, AM_DEFAULT);
}

ShipEngine::ShipEngine(Context* context) :
	LogicComponent(context),
	type_(ShipEngine::Type::SE_Sun),
	speed_(0.f),
	rotationSpeed_(0.f)
{
}

ShipEngine::Type ShipEngine::GetEngineType() const
{
	return type_;
}

F32 ShipEngine::GetSpeed() const
{
	return speed_;
}

F32 ShipEngine::GetRotationSpeed() const
{
	return rotationSpeed_;
}
