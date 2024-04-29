#pragma once

#include "stateManager/gameStates.h"
#include "utility/simpleTypes.h"

namespace Urho3D
{
	class Button;
	class UIElement;
	class ScrollBar;
	class Text;
}

class MenuAudioPropertiesState : public IGameState
{
	URHO3D_OBJECT(MenuAudioPropertiesState, IGameState);

public:

	MenuAudioPropertiesState(Urho3D::Context * context);
	virtual ~MenuAudioPropertiesState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:
	// options list
	F32 volumeValue_;

	/// UI elements
	WeakPtr<UIElement>    window_;
	WeakPtr<ScrollBar>    volume_;
	WeakPtr<Text>         volumeValueText_;
	WeakPtr<Button>       returnToMenu_;
	WeakPtr<Button>       applyChanges_;

	// event related functions
	void SubscribeToEvents();
	void HandleVolumeChange(StringHash eventType, VariantMap& eventData);

	void HandleBackButtonClick(StringHash eventType, VariantMap& eventData);
	void HandleApplyButtonClick(StringHash eventType, VariantMap& eventData);
};
