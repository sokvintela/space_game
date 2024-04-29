#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "stateManager/playersManager.h"
#include "legacyMode/utility/lSharedData.h"
#include "prefabs/prefabsStorage.h"
#include "config.h"
#include "races/races.h"
#include "tacticalMode/gameEvents/tGameEvents.h"

#include "legacyMode/startNewGameStates/multiGalaxyState.h"

using namespace Urho3D;

GameMultiGalaxyState::GameMultiGalaxyState(Urho3D::Context * context) :
	IGameState(context),
	race_(Races::Races::R_Irrs),
	minNumOfStars_(2),
	minNumOfEnemies_(1),
	maxNumOfStars_(100),
	maxNumOfEnemies_(100),
	numberOfStars_(15),
	numberOfEnemies_(7),
	type_(Galaxy::Type::Elliptic),
	age_(Galaxy::Age::Young),
	myTeam_(0)
{
}

GameMultiGalaxyState::~GameMultiGalaxyState()
{
	PlayersManager* playersManager = GetSubsystem<PlayersManager>();
	if (playersManager)
		playersManager->connectedPlayers_.Clear();
}

void GameMultiGalaxyState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/multiGalaxySetup.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	playersList_ =        static_cast<ListView*>(uiStateRoot_->GetChild("playersList_", true));

	galaxyTypeList_ =     static_cast<DropDownList*>(uiStateRoot_->GetChild("galaxyTypeList_", true));
	galaxyAgeList_ =      static_cast<DropDownList*>(uiStateRoot_->GetChild("galaxyAgeList_", true));

	starsCountTitle_ =    static_cast<Text*>(uiStateRoot_->GetChild("starsCountTitle_", true));
	enemiesCountTitle_ =  static_cast<Text*>(uiStateRoot_->GetChild("enemiesCountTitle_", true));

	starsSlider_ =        static_cast<Slider*>(uiStateRoot_->GetChild("starsSlider_", true));
	enemiesSlider_ =      static_cast<Slider*>(uiStateRoot_->GetChild("enemiesSlider_", true));

	returnToMenu_ =       static_cast<Button*>(uiStateRoot_->GetChild("returnToMenu_", true));
	startGame_ =          static_cast<Button*>(uiStateRoot_->GetChild("nextStage_", true));
}

void GameMultiGalaxyState::Enter()
{
	uiStateRoot_->SetVisible(true);

	Network* network = GetSubsystem<Network>();
	network->RegisterRemoteEvent(RE_GAME_STARTED);
	network->RegisterRemoteEvent(RE_NEW_PLAYER);
	network->RegisterRemoteEvent(RE_REMOVE_PLAYER);
	network->RegisterRemoteEvent(RE_CLIENT_CHANGE_TEAM);
	network->RegisterRemoteEvent(RE_GAME_PARAMETERS_CHANGE);

	SubscribeToEvent(returnToMenu_, E_PRESSED, URHO3D_HANDLER(GameMultiGalaxyState, MenuButtonClick));

	LSharedData* sharedData = GetSubsystem<LSharedData>();
	sharedData->isFastGame_ = false;

	SubscribeToEvent(RE_NEW_PLAYER, URHO3D_HANDLER(GameMultiGalaxyState, HandleClientListNewPlayer));
	SubscribeToEvent(RE_REMOVE_PLAYER, URHO3D_HANDLER(GameMultiGalaxyState, HandleClientListRemovePlayer));
	SubscribeToEvent(RE_CLIENT_CHANGE_TEAM, URHO3D_HANDLER(GameMultiGalaxyState, HandleClientListPlayerTeamChange));

	Connection* serverConnection = network->GetServerConnection();
	if (!serverConnection)
	{
		PlayersManager* playersManager = GetSubsystem<PlayersManager>();
		if (!playersManager)
		{
			playersManager = new PlayersManager(context_);
			context_->RegisterSubsystem(playersManager);
		}

		Configuration* config = GetSubsystem<Configuration>();
		U32 serverPort = config->GetValue("port").GetInt();
		network->StartServer(serverPort);

		if (!sharedData->isServerWithoutPlayer_)
		{
			playersManager->CreateServerSidePlayer();
		}

		SubscribeToEvent(galaxyTypeList_, E_ITEMSELECTED, URHO3D_HANDLER(GameMultiGalaxyState, HandleGalaxyTypeChange));
		SubscribeToEvent(galaxyAgeList_, E_ITEMSELECTED, URHO3D_HANDLER(GameMultiGalaxyState, HandleGalaxyAgeChange));

		SubscribeToEvent(starsSlider_, E_SLIDERCHANGED, URHO3D_HANDLER(GameMultiGalaxyState, HandleStarsSliderChange));
		SubscribeToEvent(enemiesSlider_, E_SLIDERCHANGED, URHO3D_HANDLER(GameMultiGalaxyState, HandleEnemiesSliderChange));

		SubscribeToEvent(startGame_, E_PRESSED, URHO3D_HANDLER(GameMultiGalaxyState, StartGameButtonClick));
	}
	else
	{
		sharedData->isConnectToGame_ = true;

		sharedData->multiScene_ = new Scene(context_);
		sharedData->multiScene_->CreateComponent<Octree>(LOCAL);
		sharedData->multiScene_->CreateComponent<DebugRenderer>(LOCAL);

		// Create 2D physics world component
		PhysicsWorld2D* physicsWorld = sharedData->multiScene_->CreateComponent<PhysicsWorld2D>(LOCAL);
		physicsWorld->SetGravity(Vector2::ZERO);

		serverConnection->SetScene(sharedData->multiScene_);

		galaxyTypeList_->SetEnabled(false);
		galaxyAgeList_->SetEnabled(false);
		starsSlider_->GetParent()->SetEnabledRecursive(false);
		enemiesSlider_->GetParent()->SetEnabledRecursive(false);

		SubscribeToEvent(RE_GAME_STARTED, URHO3D_HANDLER(GameMultiGalaxyState, HandleGameStarted));
		SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(GameMultiGalaxyState, HandleServerStopped));

		SubscribeToEvent(RE_GAME_PARAMETERS_CHANGE, URHO3D_HANDLER(GameMultiGalaxyState, HandleClientGameParametersChange));
	}
}

void GameMultiGalaxyState::HandleRaceSelection(StringHash eventType, VariantMap & eventData)
{
	S32 index = eventData[ItemSelected::P_SELECTION].GetInt();
	race_ = static_cast<Races::Races>(index);
}

void GameMultiGalaxyState::HandleGalaxyTypeChange(StringHash eventType, VariantMap & eventData)
{
	type_ = static_cast<Galaxy::Type>(eventData[ItemSelected::P_SELECTION].GetInt());

	Network* network = GetSubsystem<Network>();
	network->BroadcastRemoteEvent(RE_GAME_PARAMETERS_CHANGE, true, ComposeGameParameters());
}

void GameMultiGalaxyState::HandleGalaxyAgeChange(StringHash eventType, VariantMap & eventData)
{
	age_ = static_cast<Galaxy::Age>(eventData[ItemSelected::P_SELECTION].GetInt());

	Network* network = GetSubsystem<Network>();
	network->BroadcastRemoteEvent(RE_GAME_PARAMETERS_CHANGE, true, ComposeGameParameters());
}

void GameMultiGalaxyState::HandleStarsSliderChange(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;

	numberOfStars_ = eventData[P_VALUE].GetInt() + minNumOfStars_;
	starsCountTitle_->SetText(String(numberOfStars_));

	Network* network = GetSubsystem<Network>();
	network->BroadcastRemoteEvent(RE_GAME_PARAMETERS_CHANGE, true, ComposeGameParameters());
}

void GameMultiGalaxyState::HandleEnemiesSliderChange(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;

	numberOfEnemies_ = eventData[P_VALUE].GetInt() + minNumOfEnemies_;
	enemiesCountTitle_->SetText(String(numberOfEnemies_));

	Network* network = GetSubsystem<Network>();
	network->BroadcastRemoteEvent(RE_GAME_PARAMETERS_CHANGE, true, ComposeGameParameters());
}

void GameMultiGalaxyState::MenuButtonClick(StringHash eventType, VariantMap & eventData)
{
	Network* network = GetSubsystem<Network>();
	if (network->IsServerRunning())
		network->StopServer();
	else
		network->Disconnect();

	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MENU_MULTI_GAME);
}

void GameMultiGalaxyState::StartGameButtonClick(StringHash eventType, VariantMap & eventData)
{
	LSharedData* sharedData = GetSubsystem<LSharedData>();

	if (race_ == Races::Races::R_Random)
	{
		S32 raceNumber = Urho3D::Random(static_cast<S32>(Races::Races::R_Random));
		race_ = static_cast<Races::Races>(raceNumber);
	}

	sharedData->myRace_ = race_;
	sharedData->myTeam_ = myTeam_;
	sharedData->type_ = type_;
	sharedData->age_ = age_;

	auto availableEngines = GetSubsystem<PrefabsStorage>()->GetAvailabeLegacyEquipment(LEquipment::EquipmentType::ET_Engine, nullptr);
	auto availableWeapons = GetSubsystem<PrefabsStorage>()->GetAvailabeLegacyEquipment(LEquipment::EquipmentType::ET_Weapon, nullptr);

	if(!availableEngines.Empty())
		sharedData->engine_ = availableEngines.Front().first_;

	if (!availableWeapons.Empty())
	{
		sharedData->leftWeapon_ = availableWeapons.Front().first_;
		sharedData->rightWeapon_ = availableWeapons.Front().first_;
	}

	Network* network = GetSubsystem<Network>();
	if (network->IsServerRunning())
	{
		SendEvent(G_STATE_CHANGE,
			GameChangeStateEvent::P_STATE, GameStates::LSPACE);
	}
}

void GameMultiGalaxyState::HandleClientListNewPlayer(StringHash eventType, VariantMap & eventData)
{
	U32 clientId = eventData[GalaxyNewPlayer::CLIENT_ID].GetUInt();
	U32 teamNumber = eventData[GalaxyNewPlayer::TEAM_NUMBER].GetUInt();
	bool isYou = eventData[GalaxyNewPlayer::IS_YOU].GetBool();

	InsertPlayerToListView(clientId, teamNumber, isYou);

	if (isYou)
	{
		LSharedData* sharedData = GetSubsystem<LSharedData>();
		sharedData->clientId_ = clientId;
	}
}

void GameMultiGalaxyState::HandleClientListRemovePlayer(StringHash eventType, VariantMap & eventData)
{
	U32 clientId = eventData[GalaxyRemovePlayer::CLIENT_ID].GetUInt();

	RemovePlayerFromListView(clientId);
}

void GameMultiGalaxyState::HandleClientListPlayerTeamChange(StringHash eventType, VariantMap & eventData)
{
	U32 clientId = eventData[GalaxyClientChangeTeam::CLIENT_ID].GetUInt();
	U32 teamNumber = eventData[GalaxyClientChangeTeam::TEAM_NUMBER].GetUInt();

	Network* network = GetSubsystem<Network>();
	if (network->IsServerRunning())
	{
		PlayersManager* playersManager = GetSubsystem<PlayersManager>();

		for (auto connectedPlayer : playersManager->connectedPlayers_)
		{
			if (connectedPlayer.second_.connection && connectedPlayer.first_ != clientId)
			{
				connectedPlayer.second_.connection->SendRemoteEvent(RE_CLIENT_CHANGE_TEAM, true, eventData);
			}
		}

		playersManager->connectedPlayers_[clientId].teamId = teamNumber;
	}

	for (U32 itemNum = 0; itemNum < playersList_->GetNumItems(); itemNum++)
	{
		auto item = playersList_->GetItem(itemNum);
		if (item->GetVar("ClientId").GetUInt() == clientId)
		{
			DropDownList* list = static_cast<DropDownList*>(item->GetChild("teamList_", true));
			list->SetSelection(teamNumber);
		}
	}
}

void GameMultiGalaxyState::HandleClientGameParametersChange(StringHash eventType, VariantMap & eventData)
{
	numberOfEnemies_ = eventData[GameParametersChange::ENEMIES_COUNT].GetUInt();
	numberOfStars_ = eventData[GameParametersChange::STARS_COUNT].GetUInt();
	age_ = static_cast<Galaxy::Age>(eventData[GameParametersChange::GALAXY_AGE].GetUInt());
	type_ = static_cast<Galaxy::Type>(eventData[GameParametersChange::GALAXY_TYPE].GetUInt());

	galaxyTypeList_->SetSelection(type_);
	galaxyAgeList_->SetSelection(age_);

	starsSlider_->SetValue(static_cast<F32>(numberOfStars_ - minNumOfStars_));
	starsCountTitle_->SetText(String(numberOfStars_));

	enemiesSlider_->SetValue(static_cast<F32>(numberOfEnemies_ - minNumOfEnemies_));
	enemiesCountTitle_->SetText(String(numberOfEnemies_));
}

void GameMultiGalaxyState::HandleClientSideTeamChange(StringHash eventType, VariantMap & eventData)
{
	U32 teamNumber = eventData[ItemSelected::P_SELECTION].GetUInt();

	VariantMap teamChangeData;
	teamChangeData[GalaxyClientChangeTeam::CLIENT_ID] = GetSubsystem<LSharedData>()->clientId_;
	teamChangeData[GalaxyClientChangeTeam::TEAM_NUMBER] = teamNumber;

	LSharedData* sharedData = GetSubsystem<LSharedData>();
	sharedData->SendGameEvent(RE_CLIENT_CHANGE_TEAM, teamChangeData);
	sharedData->myTeam_ = teamNumber;
}

void GameMultiGalaxyState::HandleGameStarted(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::LSPACE);
}

void GameMultiGalaxyState::HandleServerStopped(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MENU_MULTI_GAME);
}

void GameMultiGalaxyState::InsertPlayerToListView(U32 clientId, U32 teamNumber, bool isYou)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	UIElement* listBar = new UIElement(context_);

	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/parts/connectedPlayer.xml");
	listBar->LoadXML(layout->GetRoot(), style);

	Text* myPlayerText = static_cast<Text*>(listBar->GetChild("playerNameText_", true));
	DropDownList* teamList = static_cast<DropDownList*>(listBar->GetChild("teamList_", true));
	teamList->SetSelection(teamNumber);
	teamList->SetEnabled(isYou);

	myPlayerText->SetText("Client " + String(clientId));
	myPlayerText->SetStyleAuto();

	listBar->SetVar("ClientId", clientId);
	playersList_->AddItem(listBar);

	if (isYou)
	{
		SubscribeToEvent(teamList, E_ITEMSELECTED, URHO3D_HANDLER(GameMultiGalaxyState, HandleClientSideTeamChange));
	}
}

void GameMultiGalaxyState::RemovePlayerFromListView(U32 clientId)
{
	for(U32 itemNum = 0; itemNum < playersList_->GetNumItems(); itemNum++)
	{
		auto item = playersList_->GetItem(itemNum);
		if (item->GetVar("ClientId").GetUInt() == clientId)
		{
			playersList_->RemoveItem(itemNum);
			break;
		}
	}
}

VariantMap GameMultiGalaxyState::ComposeGameParameters() const
{
	VariantMap gameParamsChangeData;
	gameParamsChangeData[GameParametersChange::ENEMIES_COUNT] = numberOfEnemies_;
	gameParamsChangeData[GameParametersChange::STARS_COUNT] = numberOfStars_;
	gameParamsChangeData[GameParametersChange::GALAXY_AGE] = age_;
	gameParamsChangeData[GameParametersChange::GALAXY_TYPE] = type_;

	return gameParamsChangeData;
}

void GameMultiGalaxyState::Exit()
{
	playersList_->RemoveAllItems();
	uiStateRoot_->SetVisible(false);
	UnsubscribeFromAllEvents();

	Network* network = GetSubsystem<Network>();
	network->UnregisterRemoteEvent(RE_GAME_STARTED);
	network->UnregisterRemoteEvent(RE_NEW_PLAYER);
	network->UnregisterRemoteEvent(RE_REMOVE_PLAYER);
	network->UnregisterRemoteEvent(RE_CLIENT_CHANGE_TEAM);
	network->UnregisterRemoteEvent(RE_GAME_PARAMETERS_CHANGE);
}

void GameMultiGalaxyState::Pause()
{
}

void GameMultiGalaxyState::Resume()
{
}
