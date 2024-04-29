#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class LDurability : public LogicComponent
{
	URHO3D_OBJECT(LDurability, LogicComponent);

public:
	static void RegisterObject(Context* context);
	LDurability(Context* context);

	F32 GetDurability() const;
	F32 GetMaxDurability() const;
	void SetDurability(F32 durability);
	void ChangeDurability(F32 value);

	void TakeDamage(F32 damage);

private:
	F32 durability_;
	F32 maxDurability_;

	bool isFallOnDebris_;
};
