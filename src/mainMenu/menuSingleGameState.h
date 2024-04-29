#pragma once

#include "stateManager/gameStates.h"

namespace Urho3D
{
	class Button;
}

class GameMenuSingleState : public IGameState
{
	URHO3D_OBJECT(GameMenuSingleState, Component);
public:
	GameMenuSingleState(Urho3D::Context * context);
	virtual ~GameMenuSingleState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:

	void HandleStartGameClick(StringHash eventType, VariantMap& eventData);
	void HandleLoadModeClick(StringHash eventType, VariantMap& eventData);
	void HandleBackClick(StringHash eventType, VariantMap& eventData);

	WeakPtr<Button> startNewGame_;
	WeakPtr<Button> loadMode_;
	WeakPtr<Button> back_;
};
