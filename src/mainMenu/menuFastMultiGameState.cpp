#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "stateManager/playersManager.h"
#include "tacticalMode/gameEvents/tGameEvents.h"
#include "utility/sharedData.h"
#include "prefabs/prefabsStorage.h"
#include "config.h"

#include "mainMenu/menuFastMultiGameState.h"

using namespace Urho3D;

static const U32 numberOfPlayersForGameStart = 2;
static const F32 timeToLaunchGameMs_ = 20000.0f; // 20 seconds

MenuFastMultiGameState::MenuFastMultiGameState(Urho3D::Context * context)
	: IGameState(context)
	, myTeam_(0)
{
}

MenuFastMultiGameState::~MenuFastMultiGameState()
{
	PlayersManager* playersManager = GetSubsystem<PlayersManager>();
	if (playersManager)
		playersManager->connectedPlayers_.Clear();
}

void MenuFastMultiGameState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/fastMultiGalaxySetup.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	playersList_ = static_cast<ListView*>(uiStateRoot_->GetChild("playersList_", true));

	returnToMenu_ = static_cast<Button*>(uiStateRoot_->GetChild("returnToMenu_", true));
	startGame_ = static_cast<Button*>(uiStateRoot_->GetChild("nextStage_", true));
}

void MenuFastMultiGameState::Enter()
{
	uiStateRoot_->SetVisible(true);

	Network* network = GetSubsystem<Network>();
	network->RegisterRemoteEvent(RE_GAME_STARTED);
	network->RegisterRemoteEvent(RE_NEW_PLAYER);
	network->RegisterRemoteEvent(RE_REMOVE_PLAYER);
	network->RegisterRemoteEvent(RE_CLIENT_CHANGE_TEAM);

	SubscribeToEvent(returnToMenu_, E_PRESSED, URHO3D_HANDLER(MenuFastMultiGameState, MenuButtonClick));

	SubscribeToEvent(RE_NEW_PLAYER, URHO3D_HANDLER(MenuFastMultiGameState, HandleClientListNewPlayer));
	SubscribeToEvent(RE_REMOVE_PLAYER, URHO3D_HANDLER(MenuFastMultiGameState, HandleClientListRemovePlayer));

	SubscribeToEvent(RE_CLIENT_CHANGE_TEAM, URHO3D_HANDLER(MenuFastMultiGameState, HandleClientListPlayerTeamChange));

	SharedData* sharedData = GetSubsystem<SharedData>();
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

		SubscribeToEvent(startGame_, E_PRESSED, URHO3D_HANDLER(MenuFastMultiGameState, StartGameButtonClick));
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

		SubscribeToEvent(RE_GAME_STARTED, URHO3D_HANDLER(MenuFastMultiGameState, HandleGameStarted));
		SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(MenuFastMultiGameState, HandleServerStopped));
	}
}

void MenuFastMultiGameState::MenuButtonClick(StringHash eventType, VariantMap & eventData)
{
	Network* network = GetSubsystem<Network>();
	if (network->IsServerRunning())
		network->StopServer();
	else
		network->Disconnect();

	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::START_MAIN_MENU);
}

void MenuFastMultiGameState::StartGameButtonClick(StringHash eventType, VariantMap & eventData)
{
	StartGame();
}

void MenuFastMultiGameState::StartGame()
{
	SharedData* sharedData = GetSubsystem<SharedData>();
	sharedData->myTeam_ = myTeam_;

	auto availableEngines = GetSubsystem<PrefabsStorage>()->GetAvailabeEquipment(TEquipment::EquipmentType::ET_Engine, nullptr);
	auto availableWeapons = GetSubsystem<PrefabsStorage>()->GetAvailabeEquipment(TEquipment::EquipmentType::ET_Weapon, nullptr);

	if (!availableEngines.Empty())
		sharedData->engine_ = availableEngines.Front().first_;

	if (!availableWeapons.Empty())
	{
		sharedData->leftWeapon_ = availableWeapons.Front().first_;
		sharedData->rightWeapon_ = availableWeapons.Front().first_;
	}

	Network* network = GetSubsystem<Network>();
	if (network->IsServerRunning())
	{
		PlayersManager* playersManager = GetSubsystem<PlayersManager>();
		if (playersManager)
			playersManager->SetAllowConnection(false);

		SendEvent(G_STATE_CHANGE, GameChangeStateEvent::P_STATE, GameStates::TSPACE);
	}
}

void MenuFastMultiGameState::HandleClientListNewPlayer(StringHash eventType, VariantMap & eventData)
{
	U32 clientId = eventData[GalaxyNewPlayer::CLIENT_ID].GetUInt();
	U32 teamNumber = eventData[GalaxyNewPlayer::TEAM_NUMBER].GetUInt();
	bool isYou = eventData[GalaxyNewPlayer::IS_YOU].GetBool();

	InsertPlayerToListView(clientId, teamNumber, isYou);

	if (isYou)
	{
		SharedData* sharedData = GetSubsystem<SharedData>();
		sharedData->clientId_ = clientId;
	}

	PlayersManager* playersManager = GetSubsystem<PlayersManager>();
	if (playersManager && playersManager->connectedPlayers_.Size() >= numberOfPlayersForGameStart)
	{
		gameStartupTimer_.Reset();
		SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MenuFastMultiGameState, HandleTimeUpdate));
	}
}

void MenuFastMultiGameState::HandleClientListRemovePlayer(StringHash eventType, VariantMap & eventData)
{
	U32 clientId = eventData[GalaxyRemovePlayer::CLIENT_ID].GetUInt();

	RemovePlayerFromListView(clientId);
}

void MenuFastMultiGameState::HandleClientListPlayerTeamChange(StringHash eventType, VariantMap & eventData)
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

void MenuFastMultiGameState::HandleClientSideTeamChange(StringHash eventType, VariantMap & eventData)
{
	U32 teamNumber = eventData[ItemSelected::P_SELECTION].GetUInt();

	VariantMap teamChangeData;
	teamChangeData[GalaxyClientChangeTeam::CLIENT_ID] = GetSubsystem<SharedData>()->clientId_;
	teamChangeData[GalaxyClientChangeTeam::TEAM_NUMBER] = teamNumber;

	SharedData* sharedData = GetSubsystem<SharedData>();
	sharedData->SendGameEvent(RE_CLIENT_CHANGE_TEAM, teamChangeData);
	sharedData->myTeam_ = teamNumber;
}

void MenuFastMultiGameState::HandleGameStarted(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::TSPACE);
}

void MenuFastMultiGameState::HandleServerStopped(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MENU_MULTI_GAME);
}

void MenuFastMultiGameState::InsertPlayerToListView(U32 clientId, U32 teamNumber, bool isYou)
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
		SubscribeToEvent(teamList, E_ITEMSELECTED, URHO3D_HANDLER(MenuFastMultiGameState, HandleClientSideTeamChange));
	}
}

void MenuFastMultiGameState::RemovePlayerFromListView(U32 clientId)
{
	for (U32 itemNum = 0; itemNum < playersList_->GetNumItems(); itemNum++)
	{
		auto item = playersList_->GetItem(itemNum);
		if (item->GetVar("ClientId").GetUInt() == clientId)
		{
			playersList_->RemoveItem(itemNum);
			break;
		}
	}
}

void MenuFastMultiGameState::HandleTimeUpdate(StringHash eventType, VariantMap & eventData)
{
	if (gameStartupTimer_.GetMSec(false) < timeToLaunchGameMs_)
		return;

	StartGame();
}

void MenuFastMultiGameState::Exit()
{
	playersList_->RemoveAllItems();
	uiStateRoot_->SetVisible(false);
	UnsubscribeFromAllEvents();

	Network* network = GetSubsystem<Network>();
	network->UnregisterRemoteEvent(RE_GAME_STARTED);
	network->UnregisterRemoteEvent(RE_NEW_PLAYER);
	network->UnregisterRemoteEvent(RE_REMOVE_PLAYER);
	network->UnregisterRemoteEvent(RE_CLIENT_CHANGE_TEAM);
}

void MenuFastMultiGameState::Pause()
{
}

void MenuFastMultiGameState::Resume()
{
}
