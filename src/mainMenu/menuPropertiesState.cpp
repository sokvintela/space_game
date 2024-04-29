#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Network/Network.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"

#include "mainMenu/menuPropertiesState.h"

using namespace Urho3D;

MenuPropertiesState::MenuPropertiesState(Urho3D::Context * context) :
	IGameState(context)
{
}

void MenuPropertiesState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/menuProperties/menuProperties.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	// menu buttons
	video_ = static_cast<Button*>(uiStateRoot_->GetChild("video", false));
	audio_ = static_cast<Button*>(uiStateRoot_->GetChild("audio", false));
	controls_ = static_cast<Button*>(uiStateRoot_->GetChild("controls", false));
	exit_ = static_cast<Button*>(uiStateRoot_->GetChild("exit", false));
}

void MenuPropertiesState::Enter()
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (!serverConnection)
		UnsubscribeFromAllEvents();

	uiStateRoot_->SetVisible(true);
	uiStateRoot_->UpdateLayout();

	SubscribeToEvent(video_, E_PRESSED, URHO3D_HANDLER(MenuPropertiesState, HandleVideoClick));
	SubscribeToEvent(audio_, E_PRESSED, URHO3D_HANDLER(MenuPropertiesState, HandleAudioClick));
	SubscribeToEvent(controls_, E_PRESSED, URHO3D_HANDLER(MenuPropertiesState, HandleControlsClick));
	SubscribeToEvent(exit_, E_PRESSED, URHO3D_HANDLER(MenuPropertiesState, HandleExitClick));

	SubscribeToEvent(E_FOCUSED, URHO3D_HANDLER(MenuPropertiesState, HandleButtonFocused));
	SubscribeToEvent(E_DEFOCUSED, URHO3D_HANDLER(MenuPropertiesState, HandleButtonDefocused));
}


void MenuPropertiesState::HandleVideoClick(StringHash eventType, VariantMap& eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::VIDEO_PROPERTIES);
}

void MenuPropertiesState::HandleAudioClick(StringHash eventType, VariantMap& eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::AUDIO_PROPERTIES);
}

void MenuPropertiesState::HandleControlsClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::CONTROLS_PROPERTIES);
}

void MenuPropertiesState::HandleExitClick(StringHash eventType, VariantMap& eventData)
{
	SendEvent(G_STATE_EXIT_GAME);
}

void MenuPropertiesState::HandleButtonFocused(StringHash eventType, VariantMap& eventData)
{
	Button* element = static_cast<Button*>(eventData[Focused::P_ELEMENT].GetPtr());
	element->SetOpacity(0.2f);
}

void MenuPropertiesState::HandleButtonDefocused(StringHash eventType, VariantMap& eventData)
{
	Button* element = static_cast<Button*>(eventData[Focused::P_ELEMENT].GetPtr());
	element->SetOpacity(0.0f);
}

void MenuPropertiesState::Exit()
{
	uiStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void MenuPropertiesState::Pause()
{
}

void MenuPropertiesState::Resume()
{
}
