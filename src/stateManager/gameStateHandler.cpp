#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/Log.h>

#include "stateManager/gameStateEvents.h"
#include "mainMenu/startMainMenuState.h"
#include "legacyMode/space/lSpaceState.h"
#include "tacticalMode/space/tSpaceState.h"
#include "legacyMode/startNewGameStates/chooseRaceState.h"
#include "mainMenu/menuPropertiesState.h"
#include "legacyMode/startNewGameStates/galaxySetupState.h"
#include "mainMenu/menuSingleGameState.h"
#include "mainMenu/menuMultiGameState.h"
#include "mainMenu/loadingGameState.h"
#include "legacyMode/startNewGameStates/multiGalaxyState.h"
#include "mainMenu/menuConnectState.h"
#include "legacyMode/galaxy/galaxyState.h"
#include "mainMenu/menuFastMultiGameState.h"
#include "mainMenu/menuVideoPropertiesState.h"
#include "mainMenu/menuAudioPropertiesState.h"
#include "mainMenu/menuControlsPropertiesState.h"

#include "stateManager/gameStateHandler.h"

using namespace Urho3D;

GameStateHandler::GameStateHandler(Urho3D::Context* context) :
	Object(context),
	currentState_(GameStates::GameState::Count)
{
}

GameStateHandler::~GameStateHandler()
{
	RemoveAllStates();
}

void GameStateHandler::Start(U32 state)
{
	SubscribeToEvent(G_STATE_CHANGE, URHO3D_HANDLER(GameStateHandler, ChangedStateEvent));

	PushState(state);
	EnterState(state);
}

void GameStateHandler::ChangedStateEvent(StringHash eventType, VariantMap& eventData)
{
	U32 nextState = eventData[GameChangeStateEvent::P_STATE].GetUInt();

	bool destroyOldState = eventData[GameChangeStateEvent::P_DESTROY_PREVIOS_STATE].GetBool();

	ExitState(currentState_);
	if (nextState == GameStates::START_MAIN_MENU)
	{
		EnterSingleState(nextState);
	}
	else
	{
		if (destroyOldState)
			PopState(currentState_);

		if (!states_[nextState])
			PushState(nextState);

		EnterState(nextState);
	}
}

void GameStateHandler::EnterSingleState(U32 state)
{
	RemoveAllStates();

	PushState(state);
	EnterState(state);
}

std::unique_ptr<IGameState> GameStateHandler::CreateState(U32 state)
{
	std::unique_ptr<IGameState> newState = nullptr;
	switch (state)
	{
	case GameStates::GameState::START_MAIN_MENU:
		newState = std::make_unique<GameStartMainMenuState>(context_);
		break;
	case GameStates::GameState::MENU_SINGLE_GAME:
		newState = std::make_unique<GameMenuSingleState>(context_);
		break;
	case GameStates::GameState::MENU_MULTI_GAME:
		newState = std::make_unique<GameMenuMultiState>(context_);
		break;
	case GameStates::GameState::MENU_PROPERTIES:
		newState = std::make_unique<MenuPropertiesState>(context_);
		break;
	case GameStates::GameState::VIDEO_PROPERTIES:
		newState = std::make_unique<MenuVideoPropertiesState>(context_);
		break;
	case GameStates::GameState::AUDIO_PROPERTIES:
		newState = std::make_unique<MenuAudioPropertiesState>(context_);
		break;
	case GameStates::GameState::CONTROLS_PROPERTIES:
		newState = std::make_unique<MenuControlsPropertiesState>(context_);
		break;
	case GameStates::GameState::LOADING_GAME:
		newState = std::make_unique<LoadingGameState>(context_);
		break;
	case GameStates::GameState::CHOOSE_RACE:
		newState = std::make_unique<GameChooseRaceState>(context_);
		break;
	case GameStates::GameState::LSPACE:
		newState = std::make_unique<LGameSpaceState>(context_);
		break;
	case GameStates::GameState::TSPACE:
		newState = std::make_unique<TGameSpaceState>(context_);
		break;
	case GameStates::GameState::GALAXY_SETUP:
		newState = std::make_unique<GameGalaxySetupState>(context_);
		break;
	case GameStates::GameState::MULTI_GALAXY_SETUP:
		newState = std::make_unique<GameMultiGalaxyState>(context_);
		break;
	case GameStates::GameState::MENU_CONNECT_STATE:
		newState = std::make_unique<GameMenuConnectState>(context_);
		break;
	case GameStates::GameState::GALAXY_STATE:
		newState = std::make_unique<GalaxyState>(context_);
		break;
	case GameStates::GameState::MULTI_FAST:
		newState = std::make_unique<MenuFastMultiGameState>(context_);
		break;
	default:
		URHO3D_LOGERROR("Not a valid game state");
		SendEvent(G_STATE_EXIT_GAME);
		return nullptr;
	}

	newState->Create();

	return std::move(newState);
}

void GameStateHandler::PushState(U32 state)
{
	states_[state] = CreateState(state);
}

void GameStateHandler::PopState(U32 state)
{
	states_[state] = nullptr;
}

void GameStateHandler::EnterState(U32 state)
{
	currentState_ = static_cast<GameStates::GameState>(state);
	states_[state]->Enter();
}

void GameStateHandler::ExitState(U32 state)
{
	states_[state]->Exit();
}

void GameStateHandler::Pause()
{
	states_[currentState_]->Pause();
}

void GameStateHandler::Resume()
{
	states_[currentState_]->Resume();
}

void GameStateHandler::RemoveAllStates()
{
	states_.clear();
}
