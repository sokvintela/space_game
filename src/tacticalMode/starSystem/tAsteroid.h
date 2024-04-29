#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class TAsteroid : public LogicComponent
{
	URHO3D_OBJECT(TAsteroid, LogicComponent);

public:
	enum Type
	{
		Small,
		Big
	};

	static void RegisterObject(Context* context);
	TAsteroid(Context* context);
	~TAsteroid();

	void Create(Type type) const;
};