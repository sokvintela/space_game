#pragma once

#include "stateManager/gameStates.h"

namespace Urho3D
{
	class Button;
	class LineEdit;
}

class GameMenuConnectState : public IGameState
{
	URHO3D_OBJECT(GameMenuConnectState, IGameState);
public:
	GameMenuConnectState(Urho3D::Context * context);
	virtual ~GameMenuConnectState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:

	void HandleConnectClick(StringHash eventType, VariantMap& eventData);
	void HandleBackClick(StringHash eventType, VariantMap& eventData);

	WeakPtr<LineEdit> connectionText_;
	WeakPtr<Button> connect_;
	WeakPtr<Button> back_;
};
