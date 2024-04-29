#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class Crystal : public LogicComponent
{
	URHO3D_OBJECT(Crystal, LogicComponent);

public:

	static void RegisterObject(Context* context);
	Crystal(Context* context);
	~Crystal();

	void Create();

	bool IsEspecial() const;
	void SetEspeciallity();

	void SetVisible(bool visibility);

private:
	U32 type_;
	bool especiallity_;
	Color color_;

	Timer hideTimer_;
	static U32 showTimeMs_;

	virtual void Update(F32 timeStep);
};