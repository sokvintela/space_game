#include <Urho3D/UI/UI.h>

#include "stateManager/gameStates.h"

IGameState::IGameState(Urho3D::Context * context) :
	Object(context)
{
	UIElement* uiRoot = GetSubsystem<UI>()->GetRoot();
	uiStateRoot_ = uiRoot->CreateChild("UIElement");
}

ISubState::ISubState(Urho3D::Context * context, UIElement* uiSubState) :
	Object(context),
	uiSubStateRoot_(uiSubState)
{
}
