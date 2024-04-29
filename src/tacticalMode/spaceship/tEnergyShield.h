#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class TEnergyShield : public LogicComponent
{
	URHO3D_OBJECT(TEnergyShield, LogicComponent);

public:
	static void RegisterObject(Context* context);
	TEnergyShield(Context* context);
	~TEnergyShield() = default;

	F32 GetValue() const;

	F32 ReceiveDamage(F32 damage);
	void Regenerate(F32 value);

private:
	F32 value_;
	Timer waitTimer_;

	virtual void Update(F32 timeStep);
};
