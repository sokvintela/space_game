#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/ScrollBar.h>
#include <Urho3D/Audio/Audio.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "utility/sharedData.h"
#include "config.h"

#include "mainMenu/menuAudioPropertiesState.h"

using namespace Urho3D;

MenuAudioPropertiesState::MenuAudioPropertiesState(Urho3D::Context * context) :
	IGameState(context),
	volumeValue_(0.f)
{
}

void MenuAudioPropertiesState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/menuProperties/menuAudioProperties.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	window_ =           static_cast<Window*>(uiStateRoot_->GetChild("window_", true));
	volume_ =           static_cast<ScrollBar*>(uiStateRoot_->GetChild("volume_", true));
	volumeValueText_ =  static_cast<Text*>(uiStateRoot_->GetChild("volumeValueText_", true));
	returnToMenu_ =     static_cast<Button*>(uiStateRoot_->GetChild("returnToMenu_", true));
	applyChanges_ =     static_cast<Button*>(uiStateRoot_->GetChild("applyChanges_", true));
}

void MenuAudioPropertiesState::Enter()
{
	Audio* audio = GetSubsystem<Audio>();
	if (audio)
	{
		volumeValue_ = audio->GetMasterGain("effects");
		volume_->SetValue(volumeValue_);
		F32 clampedValue = Floor(volumeValue_ * 100.f) / 100.f;
		volumeValueText_->SetText(String(clampedValue));
	}

	uiStateRoot_->SetVisible(true);
	uiStateRoot_->UpdateLayout();

	SubscribeToEvents();
}

void MenuAudioPropertiesState::SubscribeToEvents()
{
	SubscribeToEvent(volume_, E_SCROLLBARCHANGED, URHO3D_HANDLER(MenuAudioPropertiesState, HandleVolumeChange));
	SubscribeToEvent(returnToMenu_, E_PRESSED, URHO3D_HANDLER(MenuAudioPropertiesState, HandleBackButtonClick));
	SubscribeToEvent(applyChanges_, E_PRESSED, URHO3D_HANDLER(MenuAudioPropertiesState, HandleApplyButtonClick));
}

void MenuAudioPropertiesState::HandleVolumeChange(StringHash eventType, VariantMap & eventData)
{
	volumeValue_ = eventData[SliderChanged::P_VALUE].GetFloat();

	F32 clampedValue = Floor(volumeValue_ * 100.f) / 100.f;
	volumeValueText_->SetText(String(clampedValue));
}

void MenuAudioPropertiesState::HandleBackButtonClick(StringHash eventType, VariantMap & eventData)
{
	bool isFromGame = GetSubsystem<SharedData>()->inGame_;

	GameStates::GameState targetState = isFromGame ?
		GameStates::TSPACE :
		GameStates::MENU_PROPERTIES;

	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, targetState);
}

void MenuAudioPropertiesState::HandleApplyButtonClick(StringHash eventType, VariantMap & eventData)
{
	Configuration* config = GetSubsystem<Configuration>();

	Audio* audio = GetSubsystem<Audio>();
	if (audio)
	{
		audio->SetMasterGain("effects", volumeValue_);
		config->SetValue("sound", volumeValue_);
	}

	config->Save();

	uiStateRoot_->UpdateLayout();
}

void MenuAudioPropertiesState::Exit()
{
	uiStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void MenuAudioPropertiesState::Pause()
{
}

void MenuAudioPropertiesState::Resume()
{
}
