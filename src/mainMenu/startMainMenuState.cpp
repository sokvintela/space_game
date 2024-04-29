#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/Log.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "utility/sharedData.h"
#include "prefabs/prefabsStorage.h"
#include "config.h"

#include "mainMenu/startMainMenuState.h"

using namespace Urho3D;

GameStartMainMenuState::GameStartMainMenuState(Urho3D::Context * context) :
	IGameState(context)
{
}

void GameStartMainMenuState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/mainMenu.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	// menu buttons
	singleGame_ =		static_cast<Button*>(uiStateRoot_->GetChild("single", false));
	fastGame_ =			static_cast<Button*>(uiStateRoot_->GetChild("fast", false));
	multiMode_ =		static_cast<Button*>(uiStateRoot_->GetChild("multi", false));
	fastMultiMode_ =	static_cast<Button*>(uiStateRoot_->GetChild("fastMulti", false));
	properties_ =		static_cast<Button*>(uiStateRoot_->GetChild("properties", false));
	bonuses_ =			static_cast<Button*>(uiStateRoot_->GetChild("bonus", false));
	exit_ =				static_cast<Button*>(uiStateRoot_->GetChild("exit", false));
}

void GameStartMainMenuState::Enter()
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (!serverConnection)
		UnsubscribeFromAllEvents();

	uiStateRoot_->SetVisible(true);
	uiStateRoot_->UpdateLayout();

	SubscribeToEvent(singleGame_, E_PRESSED, URHO3D_HANDLER(GameStartMainMenuState, HandleSingleGameClick));
	SubscribeToEvent(fastGame_, E_PRESSED, URHO3D_HANDLER(GameStartMainMenuState, HandleFastGameClick));
	SubscribeToEvent(multiMode_, E_PRESSED, URHO3D_HANDLER(GameStartMainMenuState, HandleMultiModeClick));
	SubscribeToEvent(fastMultiMode_, E_PRESSED, URHO3D_HANDLER(GameStartMainMenuState, HandleFastMultiModeClick));
	SubscribeToEvent(properties_, E_PRESSED, URHO3D_HANDLER(GameStartMainMenuState, HandlePropertiesClick));
	SubscribeToEvent(bonuses_, E_PRESSED, URHO3D_HANDLER(GameStartMainMenuState, HandleBonusesClick));
	SubscribeToEvent(exit_, E_PRESSED, URHO3D_HANDLER(GameStartMainMenuState, HandleExitClick));

	SubscribeToEvent(E_FOCUSED, URHO3D_HANDLER(GameStartMainMenuState, HandleButtonFocused));
	SubscribeToEvent(E_DEFOCUSED, URHO3D_HANDLER(GameStartMainMenuState, HandleButtonDefocused));
}


void GameStartMainMenuState::HandleSingleGameClick(StringHash eventType, VariantMap& eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MENU_SINGLE_GAME);
}

void GameStartMainMenuState::HandleMultiModeClick(StringHash eventType, VariantMap& eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MENU_MULTI_GAME);
}

void GameStartMainMenuState::HandleFastMultiModeClick(StringHash eventType, VariantMap & eventData)
{
	Network* network = GetSubsystem<Network>();
	String address = "localhost";

	Configuration* config = GetSubsystem<Configuration>();
	bool result = network->Connect(config->GetValue("address").GetString(), config->GetValue("port").GetInt(), nullptr);

	if (result)
	{
		SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(GameStartMainMenuState, HandleServerConnectFailed));
		SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(GameStartMainMenuState, HandleServerConnected));
	}
}

void GameStartMainMenuState::HandleFastGameClick(StringHash eventType, VariantMap & eventData)
{
	SharedData* sharedData = GetSubsystem<SharedData>();
	sharedData->myRace_ = Races::R_Chinchi;

	PrefabsStorage* prefabs = GetSubsystem<PrefabsStorage>();
	sharedData->engine_ = "Sun";//GetAvailabeEquipment(Equipment::ET_Engine, nullptr).Begin()->first_;
	sharedData->leftWeapon_ = "Laser"; // prefabs->GetAvailabeEquipment(TEquipment::ET_Weapon, nullptr).Begin()->first_;
	sharedData->rightWeapon_ = prefabs->GetAvailabeEquipment(TEquipment::ET_Weapon, nullptr).Begin()->first_;

	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::TSPACE);
}

void GameStartMainMenuState::HandlePropertiesClick(StringHash eventType, VariantMap& eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MENU_PROPERTIES);
}

void GameStartMainMenuState::HandleBonusesClick(StringHash eventType, VariantMap& eventData)
{
}

void GameStartMainMenuState::HandleExitClick(StringHash eventType, VariantMap& eventData)
{
	SendEvent(G_STATE_EXIT_GAME);
}

void GameStartMainMenuState::HandleButtonFocused(StringHash eventType, VariantMap& eventData)
{
	Button* element = static_cast<Button*>(eventData[Focused::P_ELEMENT].GetPtr());
	element->SetOpacity(0.2f);
}

void GameStartMainMenuState::HandleButtonDefocused(StringHash eventType, VariantMap& eventData)
{
	Button* element = static_cast<Button*>(eventData[Focused::P_ELEMENT].GetPtr());
	element->SetOpacity(0.0f);
}

void GameStartMainMenuState::HandleServerConnected(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE, GameChangeStateEvent::P_STATE, GameStates::MULTI_FAST);
}

void GameStartMainMenuState::HandleServerConnectFailed(StringHash eventType, VariantMap & eventData)
{
	URHO3D_LOGINFO("connect failed");
}

void GameStartMainMenuState::Exit()
{
	uiStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void GameStartMainMenuState::Pause()
{
}

void GameStartMainMenuState::Resume()
{
}
