#pragma once

#include "stateManager/gameStates.h"

namespace Urho3D
{
	class Button;
}

class MenuPropertiesState : public IGameState
{
	URHO3D_OBJECT(MenuPropertiesState, Component);
public:
	MenuPropertiesState(Urho3D::Context * context);
	virtual ~MenuPropertiesState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:

	void HandleVideoClick(StringHash eventType, VariantMap& eventData);
	void HandleAudioClick(StringHash eventType, VariantMap& eventData);
	void HandleControlsClick(StringHash eventType, VariantMap& eventData);
	void HandleExitClick(StringHash eventType, VariantMap& eventData);
	void HandleButtonFocused(StringHash eventType, VariantMap& eventData);
	void HandleButtonDefocused(StringHash eventType, VariantMap& eventData);

	WeakPtr<Button> video_;
	WeakPtr<Button> audio_;
	WeakPtr<Button> controls_;
	WeakPtr<Button> exit_;
};
