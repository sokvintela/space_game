#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/Urho2D/PhysicsWorld2D.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/PhysicsEvents2D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include "utility/sharedData.h"
#include "mainMenu/spaceMainMenuState.h"
#include "strategicMode/SubStates/sSpacePlanetState.h"
#include "strategicMode/SubStates/sSpaceShipInfoState.h"
#include "strategicMode/SubStates/sSavingGameState.h"
#include "legacyMode/playerLogic/lResearchTree.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "legacyMode/gameEvents/lGameEventProcessing.h"
#include "legacyMode/playerLogic/lTeam.h"
#include "legacyMode/space/lSpaceBuildingsUI.h"
#include "legacyMode/space/lKnowledgeUI.h"
#include "legacyMode/space/lStatusLineUI.h"
#include "config.h"
#include "legacyMode/space/lFindObjectsHelper.h"
#include "utility/generalMethods.h"
#include "stateManager/gameStateEvents.h"
#include "stateManager/statesList.h"
#include "stateManager/playersManager.h"
#include "legacyMode/space/lShipEquipmentUI.h"
#include "legacyMode/space/lCollisionHandler.h"
#include "tacticalMode/gameEvents/tGameEvents.h"

#include "strategicMode/sSpaceState.h"

using namespace Urho3D;

void SGameSpaceState::RegisterComponents(Context * context)
{
	SGameShipInfoState::RegisterEvents(context);
	SGameSpacePlanetState::RegisterEvents(context);
}

void SGameSpaceState::RegisterEvents(Context * context)
{
	Network* network = context->GetSubsystem<Network>();
	network->RegisterRemoteEvent(RE_OBJECT_BUILD);
	network->RegisterRemoteEvent(RE_DETECTOR_USE);
	network->RegisterRemoteEvent(RE_HYPER_DRIVE);
	network->RegisterRemoteEvent(RE_RESEARCH_SELECTION);
	network->RegisterRemoteEvent(RE_RESEARCH_COMPLETED);
	network->RegisterRemoteEvent(RE_GAME_OVER);
}

SGameSpaceState::SGameSpaceState(Urho3D::Context * context) :
	IGameState(context),
	activeSubState_(SubStates::Count),
	drawDebug_(false)
{
	SetRandomSeed(Time::GetSystemTime());
}

SGameSpaceState::~SGameSpaceState()
{
	SharedData* sharedData = GetSubsystem<SharedData>();
	if (sharedData)
	{
		sharedData->inGame_ = false;
		sharedData->isLoadGame_ = false;
		sharedData->isConnectToGame_ = false;
	}

	scene_->Remove();
	context_->RemoveSubsystem<LGameEventProcessing>();
}

void SGameSpaceState::Enter()
{
	SharedData* sharedData = GetSubsystem<SharedData>();
	if (!sharedData->inGame_)
	{
		if (sharedData->isLoadGame_)
			LoadGame();
		else if (sharedData->isConnectToGame_)
			EnterGame();
		else
			CreateGame();

		CreateSubstates();
	}

	sharedData->inGame_ = true;

	uiStateRoot_->SetVisible(true);

	Graphics* graphics = GetSubsystem<Graphics>();
	if (graphics)
	{
		uiStateRoot_->SetSize(graphics->GetWidth(), graphics->GetHeight());

		if (uiSpaceState_)
			uiSpaceState_->SetSize(graphics->GetWidth(), graphics->GetHeight());
	}
	scene_->SetUpdateEnabled(true);

	// Setup the viewport for displaying the scene
	SetupViewport();

	// Hook up to the frame update events
	SubscribeToEvents();

	if (activeSubState_ != SubStates::Count)
		EnterSubstate(static_cast<S32>(activeSubState_));
}

void SGameSpaceState::Exit()
{
	uiStateRoot_->SetVisible(false);
	scene_->SetEnabled(false);
	scene_->SetUpdateEnabled(false);

	Renderer* renderer = GetSubsystem<Renderer>();
	renderer->SetViewport(0, nullptr);

	UnsubscribeFromAllEvents();
}

void SGameSpaceState::Pause()
{
	//scene_->SetEnabled(false);
	//scene_->SetUpdateEnabled(false);
	uiSpaceState_->SetVisible(false);

	UnsubscribeFromEvent(menuButton_, E_PRESSED);
	UnsubscribeFromEvent(knowledgeButton_, E_PRESSED);
	UnsubscribeFromEvent(shipInfoButton_, E_PRESSED);
	UnsubscribeFromEvent(planetButton_, E_PRESSED);
	UnsubscribeFromEvent(toPlanetButton_, E_PRESSED);
	//UnsubscribeFromAllEvents();
}

void SGameSpaceState::Resume()
{
	//scene_->SetEnabled(true);
	//scene_->SetUpdateEnabled(true);
	uiSpaceState_->SetVisible(true);

	SubscribeToEvents();
}

void SGameSpaceState::EnterSubstate(S32 stateNum)
{
	activeSubState_ = static_cast<SubStates>(stateNum);
	Pause();

	Node* targetNode = nullptr;
	switch (activeSubState_)
	{
		case SubStates::Menu:
		case SubStates::SaveGame:
			break;
		case SubStates::ShipInfo:
			targetNode = LGeneralMethods::GetMyControlNode(scene_);
			break;
		case SubStates::Planet:
		{
			if (findObjectsHelper_)
				targetNode = findObjectsHelper_->GetEnteredNode();

			break;
		}
		default:
			break;
	}

	subStates_[stateNum]->Enter(targetNode);
}

void SGameSpaceState::ExitSubstate()
{
	SharedData* sharedData = GetSubsystem<SharedData>();
	if (!sharedData->saveFileName_.Empty())
	{
		File saveFile(context_, sharedData->saveFileName_, FILE_WRITE);
		bool result = scene_->SaveXML(saveFile);

		sharedData->saveFileName_ = "";

		String resultText = "save " + String(result ? "successfull" : "failed");
		SendEvent(RE_INGAME_INFO_STRING, GameInfo::INFO_STRING, resultText);
	}

	activeSubState_ = SubStates::Count;
	Resume();
}

void SGameSpaceState::CreateGame()
{
	scene_ = new Scene(context_);
	scene_->CreateComponent<Octree>(LOCAL);
	scene_->CreateComponent<DebugRenderer>(LOCAL);

	// Create 2D physics world component
	PhysicsWorld2D* physicsWorld = scene_->CreateComponent<PhysicsWorld2D>(LOCAL);
	physicsWorld->SetGravity(Vector2::ZERO);

	CreateGalaxy();
	SetupCamera();

	Network* network = GetSubsystem<Network>();
	if (network->IsServerRunning())
	{
		auto clientConnections = network->GetClientConnections();
		for (U32 con = 0; con < clientConnections.Size(); con++)
		{
			clientConnections[con]->SetScene(scene_);
		}
	}

	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(SGameSpaceState, HandleClientSceneLoaded));
	SubscribeToEvent(RE_REMOVE_PLAYER, URHO3D_HANDLER(SGameSpaceState, HandlePlayerRemoved));

	LGameEventProcessing* gameEventProcessing = new LGameEventProcessing(context_);
	gameEventProcessing->SetScene(scene_);
	gameEventProcessing->SubscribeToNetworkEvents();

	context_->RegisterSubsystem(gameEventProcessing);

	Node* controlNode = LGeneralMethods::GetMyControlNode(scene_);
	if (!controlNode)
		return;

	if (!GetSubsystem<SharedData>()->isServerWithoutPlayer_)
	{
		shipEquipment_ = MakeShared<LShipEquipment>(context_, controlNode, uiSpaceState_);
		spaceBuildings_ = MakeShared<LSpaceBuildings>(context_, controlNode, uiSpaceState_);
		knowledgeUI_ = MakeShared<LKnowledgeUI>(context_, controlNode, uiSpaceState_);
		statusLineUI_ = MakeShared<LStatusLineUI>(context_, controlNode, uiSpaceState_);
		findObjectsHelper_ = MakeShared<LFindObjectsHelper>(context_, controlNode, uiSpaceState_);
	}
}

void SGameSpaceState::LoadGame()
{
	scene_ = new Scene(context_);

	SharedData* sharedData = GetSubsystem<SharedData>();
	File loadFile(context_, sharedData->loadFileName_, FILE_READ);
	scene_->LoadXML(loadFile);

	camera_ = scene_->GetComponent<Camera>(true);
	camera_->SetViewMask(DEFAULT_VIEWMASK & ~LHIDDEN_EQIUPMENT_VIEW_MASK);

	LGameEventProcessing* gameEventProcessing = new LGameEventProcessing(context_);
	gameEventProcessing->SetScene(scene_);
	gameEventProcessing->SubscribeToNetworkEvents();

	context_->RegisterSubsystem(gameEventProcessing);

	Node* controlNode = LGeneralMethods::GetMyControlNode(scene_);
	if (!controlNode)
		return;

	shipEquipment_ = MakeShared<LShipEquipment>(context_, controlNode, uiSpaceState_);
	spaceBuildings_ = MakeShared<LSpaceBuildings>(context_, controlNode, uiSpaceState_);
	knowledgeUI_ = MakeShared<LKnowledgeUI>(context_, controlNode, uiSpaceState_);
	statusLineUI_ = MakeShared<LStatusLineUI>(context_, controlNode, uiSpaceState_);
	findObjectsHelper_ = MakeShared<LFindObjectsHelper>(context_, controlNode, uiSpaceState_);
}

void SGameSpaceState::EnterGame()
{
	SharedData* sharedData = GetSubsystem<SharedData>();
	scene_ = sharedData->multiScene_;

	SetupCamera();

	Node* controlNode = LGeneralMethods::GetMyControlNode(scene_);
	if (!controlNode)
		return;

	shipEquipment_ = MakeShared<LShipEquipment>(context_, controlNode, uiSpaceState_);
	spaceBuildings_ = MakeShared<LSpaceBuildings>(context_, controlNode, uiSpaceState_);
	knowledgeUI_ = MakeShared<LKnowledgeUI>(context_, controlNode, uiSpaceState_);
	statusLineUI_ = MakeShared<LStatusLineUI>(context_, controlNode, uiSpaceState_);
	findObjectsHelper_ = MakeShared<LFindObjectsHelper>(context_, controlNode, uiSpaceState_);
}

void SGameSpaceState::CreateSubstates()
{
	Node* controlNode = LGeneralMethods::GetMyControlNode(scene_);

	for (U32 i = 0; i < static_cast<S32>(SubStates::Count); i++)
	{
		UIElement* uiElem = nullptr;
		ISubState* subState = nullptr;
		Node* nodeToSubstate = nullptr;
		switch (static_cast<SubStates>(i))
		{
			case SubStates::Menu:
			{
				uiElem = uiStateRoot_->CreateChild<UIElement>("menuElements", i + 1);
				subState = new GameSpaceMainMenuState(context_, uiElem);
				break;
			}
			case SubStates::SaveGame:
			{
				uiElem = uiStateRoot_->CreateChild<UIElement>("saveGame", i + 1);
				subState = new SSavingGameState(context_, uiElem);
				break;
			}
			case SubStates::Planet:
			{
				uiElem = uiStateRoot_->CreateChild<UIElement>("planetElements", i + 1);
				subState = new SGameSpacePlanetState(context_, uiElem);
				nodeToSubstate = controlNode;
				break;
			}
			case SubStates::ShipInfo:
			{
				uiElem = uiStateRoot_->CreateChild<UIElement>("shipElements", i + 1);
				subState = new SGameShipInfoState(context_, uiElem);
				break;
			}
		}

		subState->parentState_ = this;
		subState->Create(nodeToSubstate);

		subStates_.Push(subState);
	}
}

void SGameSpaceState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	uiStateRoot_->SetDefaultStyle(style);

	uiSpaceState_ = uiStateRoot_->CreateChild<UIElement>("uiSpaceState_");
	uiSpaceState_->SetDefaultStyle(style);

	Graphics* graphics = GetSubsystem<Graphics>();
	if (graphics)
		uiSpaceState_->SetSize(graphics->GetWidth(), graphics->GetHeight());
	
	// GUI related
	CreateUI();
}

void SGameSpaceState::SetupCamera()
{
	Graphics* graphics = GetSubsystem<Graphics>();
	if (!graphics)
		return;

	Node* controlNode = LGeneralMethods::GetMyControlNode(scene_);
	if (!controlNode)
		return;

	Node* cameraNode = controlNode->CreateChild("camera", LOCAL);
	if (!cameraNode)
		return;

	camera_ = cameraNode->CreateComponent<Camera>();
	if (!camera_)
		return;

	camera_->SetOrthographic(true);
	camera_->SetViewMask(DEFAULT_VIEWMASK & ~LHIDDEN_EQIUPMENT_VIEW_MASK);

	camera_->SetOrthoSize((F32)graphics->GetHeight() * PIXEL_SIZE);
	camera_->SetZoom(0.008f * Min((F32)graphics->GetWidth() / 1280.0f, (F32)graphics->GetHeight() / 800.0f)); // Set zoom according to user's resolution to ensure full visibility (initial zoom (1.2) is set for full visibility at 1280x800 resolution)
	camera_->SetFarClip(300.0f);

	// Set an initial position for the front camera scene node above the plane
	cameraNode->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
}

void SGameSpaceState::SetupViewport() const
{
	Renderer* renderer = GetSubsystem<Renderer>();
	if (!renderer)
		return;

	// Set up the front camera viewport
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camera_));
	renderer->SetViewport(0, viewport);
}

void SGameSpaceState::CreateUI()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("LegacyUI/spaceState.xml");
	uiSpaceState_->LoadXML(layout->GetRoot(), style);

	// UPPER GUI
	menuButton_ = static_cast<Button*>    (uiSpaceState_->GetChild("menuButton_", true));
	knowledgeButton_ = static_cast<Button*>    (uiSpaceState_->GetChild("knowledgeButton_", true));
	shipInfoButton_ = static_cast<Button*>    (uiSpaceState_->GetChild("shipInfoButton_", true));
	planetButton_ = static_cast<Button*>    (uiSpaceState_->GetChild("planetButton_", true));

	// MIDDLE UI
	gameInfoText_ = static_cast<Text*>        (uiSpaceState_->GetChild("gameInfoText_", true));

	// LOWER GUI
	objectInfoWindow_ = uiStateRoot_->CreateChild<UIElement>();

	XMLFile* layoutPlanet = cache->GetResource<XMLFile>("UI/parts/objectInfo.xml");
	objectInfoWindow_->LoadXML(layoutPlanet->GetRoot(), style);
	objectInfoWindow_->SetVisible(false);

	toPlanetButton_ = static_cast<Button*>(objectInfoWindow_->GetChild("toPlanetButton_", true));
	toPlanetButton_->SetVisible(false);
}

void SGameSpaceState::CreateBackground() const
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	Node* backgroundNode = scene_->CreateChild("Plane");
	//backgroundNode->SetScale2D(FieldBounds::XBounds.x_, FieldBounds::YBounds.y_);
	Sprite2D* cosmosBackgroundSprite = cache->GetResource<Sprite2D>("Urho2D/back1.jpg");

	StaticSprite2D* cosmosBackground = backgroundNode->CreateComponent<StaticSprite2D>();
	cosmosBackground->SetSprite(cosmosBackgroundSprite);
}

void SGameSpaceState::CreateGalaxy() const
{
	SharedData* sharedData = GetSubsystem<SharedData>();

	Node* galaxyNode = scene_->CreateChild();
	/*
	Galaxy* galaxy = galaxyNode->CreateComponent<Galaxy>();
	galaxy->Generate(sharedData->type_, sharedData->age_, sharedData->numberOfStars_);
	galaxy->CreatePlayers(sharedData->numberOfEnemies_);
	*/
}

void SGameSpaceState::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;

	if (!camera_)
		return;

	camera_->GetNode()->SetWorldRotation2D(0.0f);

	if (gameInfoText_->IsVisible() && gameInfoTimer_.GetMSec(false) > 3000u)
		gameInfoText_->SetVisible(false);

	if (statusLineUI_)
	{
		statusLineUI_->UpdateShipInfo();
	}

	if (!findObjectsHelper_)
		return;

	findObjectsHelper_->FindClosestObjects(objectInfoWindow_, GeneralMethods::GetMouseWorldPositionXY(camera_));

	Node* controlNode = GeneralMethods::GetMyControlNode(scene_);
	if (!controlNode)
		return;

	Node* enteredNode = findObjectsHelper_->GetEnteredNode();
	if (enteredNode)
	{
		/*
		// TDOO owner team
		S32	objectTeamNumber = GeneralMethods::GetOwnerTeamNumber(enteredNode);
		S32	playerTeamNumber = GeneralMethods::GetOwnerTeamNumber(controlNode);

		if (objectTeamNumber == LTeam::NoTeamId || playerTeamNumber == objectTeamNumber)
			spaceBuildings_->UpdateBuildingsUI(enteredNode);
			*/
	}
	else
	{
		spaceBuildings_->SwitchOff();
	}

	Node* hoveringNode = findObjectsHelper_->GetHoveringNode();
	if (hoveringNode)
	{
		IntVector2 position = GeneralMethods::WorldToScreen(camera_, hoveringNode->GetWorldPosition());

		objectInfoWindow_->SetVisible(true);
		objectInfoWindow_->SetPosition(position.x_, position.y_);
	}

	F32 timeStep = eventData[P_TIMESTEP].GetFloat();
}

void SGameSpaceState::HandleResearchUpdate(StringHash eventType, VariantMap& eventData)
{
	U32 researchedItem = eventData[ResearchCompletedEvent::RESEARCH_TYPE].GetUInt();
	U32 teamNumber = eventData[ResearchCompletedEvent::TEAM_NUMBER].GetUInt();

	LTeam* team = LGeneralMethods::GetTeam(scene_, GetSubsystem<SharedData>()->myTeam_);
	if (!team)
		return;

	LResearchTree* research = team->GetComponent<LResearchTree>();
	if (!research)
		return;

	knowledgeUI_->HandleUpdateKnowledges();

	String infoString = research->GetResearchName(researchedItem) + " has been researched!";
	SendEvent(RE_INGAME_INFO_STRING, GameInfo::INFO_STRING, infoString);

	spaceBuildings_->UpdateBuildingsUI(findObjectsHelper_->GetEnteredNode());
}

void SGameSpaceState::HandleGameOver(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::START_MAIN_MENU,
		GameChangeStateEvent::P_DESTROY_PREVIOS_STATE, true);
}

void SGameSpaceState::SubscribeToEvents()
{
	SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(SGameSpaceState, HandlePhysicsPreStep));

	// Subscribe key down event
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(SGameSpaceState, HandleKeyDown));

	// Subscribe HandleUpdate() function for processing update events
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(SGameSpaceState, HandleUpdate));

	SubscribeToEvent(RE_PLANET_OPEN, URHO3D_HANDLER(SGameSpaceState, HandlePlanetClick));

	SubscribeToEvent(RE_INGAME_INFO_STRING, URHO3D_HANDLER(SGameSpaceState, HandleIngameInfo));
	SubscribeToEvent(menuButton_, E_PRESSED, URHO3D_HANDLER(SGameSpaceState, HandleMenuClick));
	SubscribeToEvent(knowledgeButton_, E_PRESSED, URHO3D_HANDLER(SGameSpaceState, HandleKnowledgeClick));

	SubscribeToEvent(shipInfoButton_, E_PRESSED, URHO3D_HANDLER(SGameSpaceState, HandleShipInfoClick));
	SubscribeToEvent(planetButton_, E_PRESSED, URHO3D_HANDLER(SGameSpaceState, HandlePlanetClick));

	SubscribeToEvent(E_MOUSEWHEEL, URHO3D_HANDLER(SGameSpaceState, HandleZoomCamera));

	// Subscribe HandlePostRenderUpdate() function for processing the post-render update event, during which we request debug geometry
	SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(SGameSpaceState, HandlePostRenderUpdate));

	SubscribeToEvent(RE_RESEARCH_COMPLETED, URHO3D_HANDLER(SGameSpaceState, HandleResearchUpdate));

	SubscribeToEvent(RE_GAME_OVER, URHO3D_HANDLER(SGameSpaceState, HandleGameOver));
}

void SGameSpaceState::HandlePhysicsPreStep(StringHash eventType, VariantMap & eventData)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	UI* ui = GetSubsystem<UI>();
}

void SGameSpaceState::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;

	S32 key = eventData[P_KEY].GetInt();

	// Close console (if open) or exit when ESC is pressed
	if (key == KEY_ESCAPE)
		EnterSubstate(static_cast<int>(SubStates::Menu));

	// Toggle physics debug geometry with space
	if (key == KEY_SPACE)
		drawDebug_ = !drawDebug_;

	if (key == KEY_F5)
	{
		FileSystem* filesystem = GetSubsystem<FileSystem>();
		String savesPath = filesystem->GetProgramDir() + "Saves/";
		if (!filesystem->DirExists(savesPath))
			filesystem->CreateDir(savesPath);

		File saveFile(context_, savesPath + "quick.xml", FILE_WRITE);
		bool result = scene_->SaveXML(saveFile);

		String resultText = "quicksave " + String(result ? "successfull" : "failed");
		SendEvent(RE_INGAME_INFO_STRING, GameInfo::INFO_STRING, resultText);
	}
}

void SGameSpaceState::HandleIngameInfo(StringHash eventType, VariantMap& eventData)
{
	String gameInfoString = eventData[GameInfo::INFO_STRING].GetString();
	gameInfoText_->SetText(gameInfoString);
	gameInfoText_->SetVisible(true);
	gameInfoTimer_.Reset();
}

void SGameSpaceState::HandleZoomCamera(StringHash eventType, VariantMap& eventData)
{
	if (!camera_)
		return;

	Input* input = GetSubsystem<Input>();

	F32 zoom = 1.0f + Sign(input->GetMouseMoveWheel()) * 0.05f;
	camera_->SetZoom(camera_->GetZoom() * zoom);
}

void SGameSpaceState::HandleMenuClick(StringHash eventType, VariantMap & eventData)
{
	EnterSubstate(static_cast<int>(SubStates::Menu));
}

void SGameSpaceState::HandleKnowledgeClick(StringHash eventType, VariantMap & eventData)
{
	knowledgeUI_->SwitchKnowledgeUI();
}

void SGameSpaceState::HandleShipInfoClick(StringHash eventType, VariantMap & eventData)
{
	EnterSubstate(static_cast<int>(SubStates::ShipInfo));
}

void SGameSpaceState::HandlePlanetClick(StringHash eventType, VariantMap & eventData)
{
	EnterSubstate(static_cast<int>(SubStates::Planet));
}

void SGameSpaceState::HandleToPlanetClick(StringHash eventType, VariantMap & eventData)
{
	if (findObjectsHelper_->GetEnteredNode())
	{
		findObjectsHelper_->SetEnteredNode(findObjectsHelper_->GetEnteredNode());
		planetButton_->SetVisible(true);
	}
	else
	{
		findObjectsHelper_->SetEnteredNode(nullptr);
		planetButton_->SetVisible(false);
	}
}

void SGameSpaceState::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
	PhysicsWorld2D* physicsWorld = scene_->GetComponent<PhysicsWorld2D>();
	if (drawDebug_) physicsWorld->DrawDebugGeometry();
}

void SGameSpaceState::HandleClientSceneLoaded(StringHash eventType, VariantMap & eventData)
{
	Connection* clientConnection = static_cast<Connection*>(eventData[ClientSceneLoaded::P_CONNECTION].GetPtr());
	clientConnection->SendRemoteEvent(RE_GAME_STARTED, true);
}

void SGameSpaceState::HandlePlayerRemoved(StringHash eventType, VariantMap & eventData)
{
	U32 clientId = eventData[GalaxyRemovePlayer::CLIENT_ID].GetUInt();
	U32 teamNumber = eventData[GalaxyRemovePlayer::TEAM_NUMBER].GetUInt();

	Node* playerNode = LGeneralMethods::GetClientNode(scene_, clientId, teamNumber);
	if (playerNode)
	{
		playerNode->Remove();
	}
}
