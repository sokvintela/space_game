#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"

#include "mainMenu/menuSingleGameState.h"

using namespace Urho3D;

GameMenuSingleState::GameMenuSingleState(Urho3D::Context * context) :
	IGameState(context)
{
}

void GameMenuSingleState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/menuSingle.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	startNewGame_ = static_cast<Button*>(uiStateRoot_->GetChild("startNewGame_", true));
	loadMode_ =     static_cast<Button*>(uiStateRoot_->GetChild("loadMode_", true));
	back_ =         static_cast<Button*>(uiStateRoot_->GetChild("back_", true));
}

void GameMenuSingleState::Enter()
{
	uiStateRoot_->SetVisible(true);
	uiStateRoot_->UpdateLayout();

	SubscribeToEvent(startNewGame_, E_PRESSED, URHO3D_HANDLER(GameMenuSingleState, HandleStartGameClick));
	SubscribeToEvent(loadMode_, E_PRESSED, URHO3D_HANDLER(GameMenuSingleState, HandleLoadModeClick));
	SubscribeToEvent(back_, E_PRESSED, URHO3D_HANDLER(GameMenuSingleState, HandleBackClick));
}

void GameMenuSingleState::HandleStartGameClick(StringHash eventType, VariantMap& eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::CHOOSE_RACE);
}

void GameMenuSingleState::HandleLoadModeClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::LOADING_GAME);
}

void GameMenuSingleState::HandleBackClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::START_MAIN_MENU);
}

void GameMenuSingleState::Exit()
{
	uiStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void GameMenuSingleState::Pause()
{
}

void GameMenuSingleState::Resume()
{
}
