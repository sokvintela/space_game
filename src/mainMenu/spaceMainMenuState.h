#pragma once

#include "stateManager/gameStates.h"

namespace Urho3D
{
	class Button;
}

class GameSpaceMainMenuState : public ISubState
{
	URHO3D_OBJECT(GameSpaceMainMenuState, ISubState);

public:
	GameSpaceMainMenuState(Urho3D::Context * context, UIElement * uiSubState);
	virtual ~GameSpaceMainMenuState() = default;

	virtual void Create(Node* targetNode = nullptr);
	virtual void Enter(Node* targetNode = nullptr);
	virtual void Exit();

private:

	void HandleContinueGameClick(StringHash eventType, VariantMap& eventData);
	void HandleSaveModeClick(StringHash eventType, VariantMap& eventData);
	void HandlePropertiesClick(StringHash eventType, VariantMap& eventData);
	void HandleBonusesClick(StringHash eventType, VariantMap& eventData);
	void HandleExitClick(StringHash eventType, VariantMap& eventData);

	WeakPtr<Button> startNewGame_;
	WeakPtr<Button> saveMode_;
	WeakPtr<Button> properties_;
	WeakPtr<Button> bonuses_;
	WeakPtr<Button> exit_;
};
