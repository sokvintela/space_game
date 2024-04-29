#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"

#include "mainMenu/menuMultiGameState.h"

using namespace Urho3D;

GameMenuMultiState::GameMenuMultiState(Urho3D::Context * context) :
	IGameState(context)
{
}

void GameMenuMultiState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/menuMulti.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	// menu buttons
	startNewGame_ = static_cast<Button*>(uiStateRoot_->GetChild("startNewGame_", true));
	connectMode_ =	static_cast<Button*>(uiStateRoot_->GetChild("connectMode_", true));
	loadMode_ =		static_cast<Button*>(uiStateRoot_->GetChild("loadMode_", true));
	back_ =			static_cast<Button*>(uiStateRoot_->GetChild("back_", true));
}

void GameMenuMultiState::Enter()
{
	uiStateRoot_->SetVisible(true);
	uiStateRoot_->UpdateLayout();

	SubscribeToEvent(startNewGame_, E_PRESSED, URHO3D_HANDLER(GameMenuMultiState, HandleStartGameClick));
	SubscribeToEvent(connectMode_, E_PRESSED, URHO3D_HANDLER(GameMenuMultiState, HandleConnectClick));
	SubscribeToEvent(loadMode_, E_PRESSED, URHO3D_HANDLER(GameMenuMultiState, HandleLoadGameClick));
	SubscribeToEvent(back_, E_PRESSED, URHO3D_HANDLER(GameMenuMultiState, HandleBackClick));
}

void GameMenuMultiState::HandleStartGameClick(StringHash eventType, VariantMap& eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MULTI_GALAXY_SETUP);
}

void GameMenuMultiState::HandleConnectClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MENU_CONNECT_STATE);
}

void GameMenuMultiState::HandleLoadGameClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::LOADING_GAME);
}

void GameMenuMultiState::HandleBackClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::START_MAIN_MENU);
}

void GameMenuMultiState::Exit()
{
	uiStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void GameMenuMultiState::Pause()
{
}

void GameMenuMultiState::Resume()
{
}
