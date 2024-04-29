#pragma once

using namespace Urho3D;

#include "legacyMode/galaxy/spaceObjects/lStar.h"

class Pulsar : public LStar
{
	URHO3D_OBJECT(Pulsar, LStar);

public:

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }
	Pulsar(Context* context);

	//void SetScale(float start_scale, float end_scale);
	void ChangeEnergySourceScale(F32 timeStep);
	void SetPulsarInitialScale(Vector2 scale);

private:
	F32 current_scale_;
	F32 step_;
	Vector2 pulsarInitialScale_;

	virtual void Update(F32 timeStep);
};