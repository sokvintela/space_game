#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/UI/Sprite.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "utility/sharedData.h"

#include "mainMenu/loadingGameState.h"

using namespace Urho3D;

LoadingGameState::LoadingGameState(Urho3D::Context * context) :
	IGameState(context)
{
}

void LoadingGameState::Create()
{
	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	String programDir = fileSystem->GetProgramDir();

	fileSelector_ = new FileSelector(context_);

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	fileSelector_->SetDefaultStyle(style);
	fileSelector_->SetPath(programDir + "Saves");
	fileSelector_->SetButtonTexts("load", "back");

	uiStateRoot_->SetDefaultStyle(style);
	uiStateRoot_->SetLayout(LM_FREE);

	// Create a new sprite, set it to use the texture
	Texture2D* planetModeSprite = cache->GetResource<Texture2D>("Urho2D/backgrounds/planet_back.jpg");
	Sprite* uiSprite = new Sprite(context_);
	uiSprite->SetTexture(planetModeSprite);
	uiSprite->SetSize(uiStateRoot_->GetSize());
	uiStateRoot_->AddChild(uiSprite);

	Window* fileWindow = fileSelector_->GetWindow();
	fileWindow->SetAlignment(HA_CENTER, VA_CENTER);

	uiStateRoot_->AddChild(fileWindow);
}

void LoadingGameState::Enter()
{
	SubscribeToEvent(fileSelector_->GetOKButton(), E_PRESSED, URHO3D_HANDLER(LoadingGameState, HandleLoadClick));
	SubscribeToEvent(fileSelector_->GetCancelButton(), E_PRESSED, URHO3D_HANDLER(LoadingGameState, HandleBackClick));
}

void LoadingGameState::Exit()
{
	fileSelector_ = nullptr;
	uiStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void LoadingGameState::Pause()
{
}

void LoadingGameState::Resume()
{
}

void LoadingGameState::HandleBackClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::START_MAIN_MENU);
}

void LoadingGameState::HandleLoadClick(StringHash eventType, VariantMap & eventData)
{
	SharedData* sharedData = GetSubsystem<SharedData>();
	sharedData->loadFileName_ = fileSelector_->GetPath() + fileSelector_->GetFileName();
	sharedData->isLoadGame_ = true;

	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::LSPACE);
}
