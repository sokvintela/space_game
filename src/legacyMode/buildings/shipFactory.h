#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

namespace Urho3D
{
	class Timer;
}

using namespace Urho3D;

class ShipFactory : public LogicComponent
{
	URHO3D_OBJECT(ShipFactory, LogicComponent);

public:

	static F32 creationTime_;
	static F32 createMobCost_;

	static void RegisterObject(Context* context);
	ShipFactory(Context* context);

	bool IsShipOnProduce(U32 type) const;
	void AddProduceShip(U32 typeNum);

	void SetProcessShips(const PODVector<U8>& value);
	PODVector<U8> GetProcessShips() const;
private:

	HashSet<U32> processShips_;
	Vector<Timer> creationtimer_;

	virtual void Update(F32 timeStep);
};
