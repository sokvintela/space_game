#pragma once

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

static const char* hyperEngineTypeNames[] = {
	"sun",
	"chemical",
	"ion",
	"plasma",
	"nuclear",
	"photon",
	0
};

class HyperDrive : public LogicComponent
{
	URHO3D_OBJECT(HyperDrive, LogicComponent);

public:
	enum Type
	{
		HE_Sun,
		HE_Chemical,
		HE_Ion,
		HE_Plasma,
		HE_Nuclear,
		HE_Photon,
		HE_Count
	};

	static void RegisterObject(Context* context);
	HyperDrive(Context* context);

	U32 GetEngineType() const;

	F32 GetSpeed() const;
	void SetSpeed(F32 speed);

	void SetStarSystem(U32 starSystemID);
	void TravelToPosition(Vector2 position);

private:
	Type type_;
	F32 speed_;

	bool move_;
	U32 starSystemID_;
	Vector2 targetPos_;
	F32 needDistance_;

	virtual void Update(F32 timeStep);
};
