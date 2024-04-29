#pragma once

#include "stateManager/gameStates.h"

namespace Urho3D
{
	class Button;
}

class GameMenuMultiState : public IGameState
{
	URHO3D_OBJECT(GameMenuMultiState, Component);
public:
	GameMenuMultiState(Urho3D::Context * context);
	virtual ~GameMenuMultiState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:

	void HandleStartGameClick(StringHash eventType, VariantMap& eventData);
	void HandleConnectClick(StringHash eventType, VariantMap& eventData);
	void HandleLoadGameClick(StringHash eventType, VariantMap& eventData);
	void HandleBackClick(StringHash eventType, VariantMap& eventData);

	WeakPtr<Button> startNewGame_;
	WeakPtr<Button> connectMode_;
	WeakPtr<Button> loadMode_;
	WeakPtr<Button> back_;
};
