#pragma once

#include "stateManager/gameStates.h"

class GalaxyState : public IGameState
{
	URHO3D_OBJECT(GalaxyState, Component);
public:
	GalaxyState(Urho3D::Context * context);
	virtual ~GalaxyState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:

};
