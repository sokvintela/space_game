#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

static const char* engineTypeNames[] = {
	"sun",
	"chemical",
	"ion",
	"plasma",
	"nuclear",
	"photon",
	0
};

class ShipEngine : public LogicComponent
{
	URHO3D_OBJECT(ShipEngine, LogicComponent);

public:

	enum Type
	{
		SE_Sun,
		SE_Chemical,
		SE_Ion,
		SE_Plasma,
		SE_Nuclear,
		SE_Photon,
		SE_Count
	};

	static void RegisterObject(Context* context);
	ShipEngine(Context* context);

	Type GetEngineType() const;
	F32 GetSpeed() const;
	F32 GetRotationSpeed() const;

private:
	Type type_;
	F32 speed_;
	F32 rotationSpeed_;
};
