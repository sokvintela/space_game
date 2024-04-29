#pragma once

#include "stateManager/gameStates.h"

namespace Urho3D
{
	class Button;
}

class GameStartMainMenuState : public IGameState
{
	URHO3D_OBJECT(GameStartMainMenuState, Component);
public:
	GameStartMainMenuState(Urho3D::Context * context);
	virtual ~GameStartMainMenuState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:

	void HandleSingleGameClick(StringHash eventType, VariantMap& eventData);
	void HandleFastGameClick(StringHash eventType, VariantMap& eventData);
	void HandleMultiModeClick(StringHash eventType, VariantMap& eventData);
	void HandleFastMultiModeClick(StringHash eventType, VariantMap& eventData);
	void HandlePropertiesClick(StringHash eventType, VariantMap& eventData);
	void HandleBonusesClick(StringHash eventType, VariantMap& eventData);
	void HandleExitClick(StringHash eventType, VariantMap& eventData);
	void HandleButtonFocused(StringHash eventType, VariantMap& eventData);
	void HandleButtonDefocused(StringHash eventType, VariantMap& eventData);

	void HandleServerConnected(StringHash eventType, VariantMap& eventData);
	void HandleServerConnectFailed(StringHash eventType, VariantMap& eventData);

	WeakPtr<Button> singleGame_;
	WeakPtr<Button> fastGame_;
	WeakPtr<Button> multiMode_;
	WeakPtr<Button> fastMultiMode_;
	WeakPtr<Button> properties_;
	WeakPtr<Button> bonuses_;
	WeakPtr<Button> exit_;
};
