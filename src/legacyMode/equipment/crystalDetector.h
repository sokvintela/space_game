#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

class CrystalDetector : public LogicComponent
{
	URHO3D_OBJECT(CrystalDetector, LogicComponent);

public:
	static void RegisterObject(Context* context);
	CrystalDetector(Context* context);

	bool Use() const;
};
