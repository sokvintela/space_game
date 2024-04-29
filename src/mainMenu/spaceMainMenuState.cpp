#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "tacticalMode/space/tSpaceState.h"
#include "legacyMode/space/lSpaceState.h"

#include "mainMenu/spaceMainMenuState.h"

using namespace Urho3D;

GameSpaceMainMenuState::GameSpaceMainMenuState(Urho3D::Context * context, UIElement* uiSubState) :
	ISubState(context, uiSubState)
{
}

void GameSpaceMainMenuState::Create(Node* targetNode)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/spaceMenu.xml");
	uiSubStateRoot_->LoadXML(layout->GetRoot(), style);

	startNewGame_ = static_cast<Button*>(uiSubStateRoot_->GetChild("startNewGame_", false));
	saveMode_ =		static_cast<Button*>(uiSubStateRoot_->GetChild("saveMode_", false));
	properties_ =	static_cast<Button*>(uiSubStateRoot_->GetChild("properties_", false));
	bonuses_ =		static_cast<Button*>(uiSubStateRoot_->GetChild("bonuses_", false));
	exit_ =			static_cast<Button*>(uiSubStateRoot_->GetChild("exit_", false));
}

void GameSpaceMainMenuState::Enter(Node* targetNode)
{
	uiSubStateRoot_->SetVisible(true);
	uiSubStateRoot_->UpdateLayout();

	SubscribeToEvent(startNewGame_, E_PRESSED, URHO3D_HANDLER(GameSpaceMainMenuState, HandleContinueGameClick));
	SubscribeToEvent(saveMode_, E_PRESSED, URHO3D_HANDLER(GameSpaceMainMenuState, HandleSaveModeClick));
	SubscribeToEvent(properties_, E_PRESSED, URHO3D_HANDLER(GameSpaceMainMenuState, HandlePropertiesClick));
	SubscribeToEvent(bonuses_, E_PRESSED, URHO3D_HANDLER(GameSpaceMainMenuState, HandleBonusesClick));
	SubscribeToEvent(exit_, E_PRESSED, URHO3D_HANDLER(GameSpaceMainMenuState, HandleExitClick));
}


void GameSpaceMainMenuState::HandleContinueGameClick(StringHash eventType, VariantMap& eventData)
{
	Exit();
	parentState_->Resume();
}

void GameSpaceMainMenuState::HandleSaveModeClick(StringHash eventType, VariantMap & eventData)
{
	Exit();
	parentState_->EnterSubstate(static_cast<int>(LGameSpaceState::SubStates::SaveGame));
}

void GameSpaceMainMenuState::HandlePropertiesClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MENU_PROPERTIES);
}

void GameSpaceMainMenuState::HandleBonusesClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::START_MAIN_MENU,
		GameChangeStateEvent::P_DESTROY_PREVIOS_STATE, true);
}

void GameSpaceMainMenuState::HandleExitClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_EXIT_GAME);
}

void GameSpaceMainMenuState::Exit()
{
	uiSubStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}
