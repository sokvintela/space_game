#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
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
#include <Urho3D/UI/Text.h>
#include <Urho3D/Navigation/Navigable.h>
#include <Urho3D/Navigation/NavigationMesh.h>

#include "tacticalMode/equipment/tEquipment.h"
#include "utility/sharedData.h"
#include "mainMenu/spaceMainMenuState.h"
#include "tacticalMode/gameEvents/tGameEvents.h"
#include "tacticalMode/gameEvents/tGameEventProcessing.h"
#include "tacticalMode/space/tStatusLineUI.h"
#include "config.h"
#include "tacticalMode/space/tFindObjectsHelper.h"
#include "utility/generalMethods.h"
#include "tacticalMode/playerLogic/tTeam.h"
#include "stateManager/gameStateEvents.h"
#include "stateManager/statesList.h"
#include "stateManager/playersManager.h"
#include "tacticalMode/playerLogic/tControlObject.h"
#include "tacticalMode/space/tShipEquipmentUI.h"
#include "tacticalMode/space/tCollisionHandler.h"
#include "tacticalMode/space/baseShipUI.h"

#include "tacticalMode/space/tSpaceState.h"

using namespace Urho3D;

void TGameSpaceState::RegisterEvents(Context * context)
{
	Network* network = context->GetSubsystem<Network>();
	network->RegisterRemoteEvent(RE_GAME_OVER);
}

TGameSpaceState::TGameSpaceState(Urho3D::Context * context) :
	IGameState(context),
	drawDebug_(false)
{
	SetRandomSeed(Time::GetSystemTime());
}

TGameSpaceState::~TGameSpaceState()
{
	SharedData* sharedData = GetSubsystem<SharedData>();
	if (sharedData)
	{
		sharedData->inGame_ = false;
		sharedData->isLoadGame_ = false;
		sharedData->isConnectToGame_ = false;
	}

	scene_->Remove();
	context_->RemoveSubsystem<TGameSpaceState>();
}

void TGameSpaceState::Enter()
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

		CreateMenuSubstate();
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
}

void TGameSpaceState::Exit()
{
	uiStateRoot_->SetVisible(false);
	scene_->SetEnabled(false);
	scene_->SetUpdateEnabled(false);

	Renderer* renderer = GetSubsystem<Renderer>();
	renderer->SetViewport(0, nullptr);

	UnsubscribeFromAllEvents();
}

void TGameSpaceState::Pause()
{
	//scene_->SetEnabled(false);
	//scene_->SetUpdateEnabled(false);
	uiSpaceState_->SetVisible(false);

	UnsubscribeFromEvent(menuButton_, E_PRESSED);
	//UnsubscribeFromAllEvents();
}

void TGameSpaceState::Resume()
{
	//scene_->SetEnabled(true);
	//scene_->SetUpdateEnabled(true);
	uiSpaceState_->SetVisible(true);

	SubscribeToEvents();
}

void TGameSpaceState::EnterMenu()
{
	if (menuSubState_)
	{
		Pause();
		menuSubState_->Enter();
	}
}

void TGameSpaceState::ExitSubstate()
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

	menuSubState_ = nullptr;
	Resume();
}

void TGameSpaceState::CreateGame()
{
	scene_ = new Scene(context_);
	scene_->CreateComponent<Octree>(LOCAL);
	scene_->CreateComponent<DebugRenderer>(LOCAL);

	// Create 2D physics world component
	PhysicsWorld2D* physicsWorld = scene_->CreateComponent<PhysicsWorld2D>(LOCAL);
	physicsWorld->SetGravity(Vector2::ZERO);

	// for navigation node should be created before world
	Node* forNavigationNode = scene_->CreateChild("ForNavigation", LOCAL);

	// Create game world
	CreateStarSystem();

	PlayersManager* playersManager = GetSubsystem<PlayersManager>();
	if (playersManager)
		playersManager->ProvideScene(scene_);

	// Navigation mesh creation, should be after game world creation
	BuildNavMesh();

	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(TGameSpaceState, HandleClientSceneLoaded));
	SubscribeToEvent(RE_REMOVE_PLAYER, URHO3D_HANDLER(TGameSpaceState, HandlePlayerRemoved));

	gameEventProcessing_ = MakeUnique<TGameEventProcessing>(context_);
	gameEventProcessing_->SetScene(scene_);

	collisionHandler_ = MakeUnique<TCollisionHandler>(context_);

	Node* controlNode = GeneralMethods::GetMyControlNode(scene_);
	if (controlNode && !GetSubsystem<SharedData>()->isServerWithoutPlayer_)
	{
		shipEquipment_ = MakeUnique<TShipEquipment>(context_, controlNode, uiSpaceState_);
		statusLineUI_ = MakeUnique<TStatusLineUI>(context_, controlNode, uiSpaceState_);
		findObjectsHelper_ = MakeUnique<TFindObjectsHelper>(context_, controlNode, objectInfoWindow_);
		baseShipUI_ = MakeUnique<BaseShipUI>(context_, controlNode, uiSpaceState_);
	}

	SetupCamera();
}

void TGameSpaceState::LoadGame()
{
	scene_ = new Scene(context_);

	SharedData* sharedData = GetSubsystem<SharedData>();
	File loadFile(context_, sharedData->loadFileName_, FILE_READ);
	scene_->LoadXML(loadFile);

	camera_ = scene_->GetComponent<Camera>(true);
	camera_->SetViewMask(DEFAULT_VIEWMASK & ~THIDDEN_EQIUPMENT_VIEW_MASK);

	gameEventProcessing_ = MakeUnique<TGameEventProcessing>(context_);
	gameEventProcessing_->SetScene(scene_);

	collisionHandler_ = MakeUnique<TCollisionHandler>(context_);

	Node* controlNode = GeneralMethods::GetMyControlNode(scene_);
	if (!controlNode)
		return;

	shipEquipment_ = MakeUnique<TShipEquipment>(context_, controlNode, uiSpaceState_);
	statusLineUI_ = MakeUnique<TStatusLineUI>(context_, controlNode, uiSpaceState_);
	findObjectsHelper_ = MakeUnique<TFindObjectsHelper>(context_, controlNode, objectInfoWindow_);
	baseShipUI_ = MakeUnique<BaseShipUI>(context_, controlNode, uiSpaceState_);
}

void TGameSpaceState::EnterGame()
{
	scene_ = GetSubsystem<SharedData>()->multiScene_;

	SetupCamera();

	Node* controlNode = GeneralMethods::GetMyControlNode(scene_);
	if (!controlNode)
		return;

	shipEquipment_ = MakeUnique<TShipEquipment>(context_, controlNode, uiSpaceState_);
	statusLineUI_ = MakeUnique<TStatusLineUI>(context_, controlNode, uiSpaceState_);
	findObjectsHelper_ = MakeUnique<TFindObjectsHelper>(context_, controlNode, objectInfoWindow_);
	baseShipUI_ = MakeUnique<BaseShipUI>(context_, controlNode, uiSpaceState_);
}

void TGameSpaceState::CreateMenuSubstate()
{
	UIElement* uiElem = uiStateRoot_->CreateChild<UIElement>("menuElements");
	menuSubState_ = new GameSpaceMainMenuState(context_, uiElem);

	menuSubState_->parentState_ = this;
	menuSubState_->Create();
}

void TGameSpaceState::Create()
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

void TGameSpaceState::SetupCamera()
{
	Graphics* graphics = GetSubsystem<Graphics>();
	if (!graphics)
		return;

	Node* cameraNode = nullptr;
	F32 zoomCoeff = 0.008f;
	if (GetSubsystem<SharedData>()->isServerWithoutPlayer_)
	{
		cameraNode = scene_->CreateChild("camera", LOCAL);
		if (!cameraNode)
			return;

		zoomCoeff = 0.015f;
	}
	else
	{
		Node* controlNode = GeneralMethods::GetMyControlNode(scene_);
		if (!controlNode)
			return;

		cameraNode = controlNode->CreateChild("camera", LOCAL);
		if (!cameraNode)
			return;
	}

	if (!cameraNode)
		return;

	camera_ = cameraNode->CreateComponent<Camera>();
	if (!camera_)
		return;

	camera_->SetOrthographic(true);
	camera_->SetViewMask(DEFAULT_VIEWMASK & ~THIDDEN_EQIUPMENT_VIEW_MASK);

	camera_->SetOrthoSize((F32)graphics->GetHeight() * PIXEL_SIZE);
	camera_->SetZoom(zoomCoeff * Min((F32)graphics->GetWidth() / 1280.0f, (F32)graphics->GetHeight() / 800.0f)); // Set zoom according to user's resolution to ensure full visibility (initial zoom (1.2) is set for full visibility at 1280x800 resolution)
	camera_->SetFarClip(300.0f);

	// Set an initial position for the front camera scene node above the plane
	cameraNode->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
}

void TGameSpaceState::SetupViewport() const
{
	Renderer* renderer = GetSubsystem<Renderer>();
	if (!renderer)
		return;

	// Set up the front camera viewport
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camera_));
	renderer->SetViewport(0, viewport);
}

void TGameSpaceState::CreateUI()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/spaceState.xml");
	uiSpaceState_->LoadXML(layout->GetRoot(), style);

	// UPPER GUI
	menuButton_ = static_cast<Button*>(uiSpaceState_->GetChild("menuButton_", true));

	// MIDDLE UI
	gameInfoText_ = static_cast<Text*>(uiSpaceState_->GetChild("gameInfoText_", true));

	// LOWER GUI
	objectInfoWindow_ = uiStateRoot_->CreateChild<UIElement>();

	XMLFile* layoutPlanet = cache->GetResource<XMLFile>("UI/parts/objectInfo.xml");
	objectInfoWindow_->LoadXML(layoutPlanet->GetRoot(), style);
	objectInfoWindow_->SetVisible(false);
}

void TGameSpaceState::BuildNavMesh() const
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	Node* forNavigationNode = scene_->GetChild("ForNavigation");
	Node* planeMeshNode = forNavigationNode->CreateChild("NavPlane");
	planeMeshNode->SetScale(500.0f);

	StaticModel* planeMesh = planeMeshNode->CreateComponent<StaticModel>(LOCAL);

	planeMesh->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
	planeMesh->SetMaterial(cache->GetResource<Material>("Materials/DefaultMaterial.xml"));

	NavigationMesh* navMesh = scene_->CreateComponent<NavigationMesh>(LOCAL);
	Navigable* navigable = scene_->CreateComponent<Navigable>(LOCAL);

	navMesh->SetCellSize(20.0f);
	navMesh->Build();
}

void TGameSpaceState::CreateBackground() const
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	Node* backgroundNode = scene_->CreateChild("Plane");
	//backgroundNode->SetScale2D(FieldBounds::XBounds.x_, FieldBounds::YBounds.y_);
	Sprite2D* cosmosBackgroundSprite = cache->GetResource<Sprite2D>("Urho2D/back1.jpg");

	StaticSprite2D* cosmosBackground = backgroundNode->CreateComponent<StaticSprite2D>();
	cosmosBackground->SetSprite(cosmosBackgroundSprite);
}

void TGameSpaceState::CreateStarSystem() const
{
	Node* systemNode = scene_->CreateChild("StarSystem");
	TStarSystem* starSystem = systemNode->CreateComponent<TStarSystem>();
	starSystem->GenerateSystemContent();
}

void TGameSpaceState::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	using namespace Update;

	if (!camera_)
		return;

	if(!dontAutoRotateCamera_)
		camera_->GetNode()->SetWorldRotation2D(0.0f);

	if (gameInfoText_->IsVisible() && gameInfoTimer_.GetMSec(false) > 3000u)
		gameInfoText_->SetVisible(false);

	if (statusLineUI_)
	{
		statusLineUI_->UpdateShipInfo();
	}

	if (!findObjectsHelper_)
		return;

	bool isCloseToBase = false;
	findObjectsHelper_->FindClosestObjects(GeneralMethods::GetMouseWorldPositionXY(camera_), isCloseToBase);

	if (isCloseToBase)
	{
		baseShipUI_->HandleUpdateUI();
	}
	else
	{
		baseShipUI_->SetVisibility(false);
	}

	Node* controlNode = GeneralMethods::GetMyControlNode(scene_);
	if (!controlNode)
		return;

	Node* hoveringNode = findObjectsHelper_->GetHoveringNode();
	if (hoveringNode)
	{
		IntVector2 position = GeneralMethods::WorldToScreen(camera_, hoveringNode->GetWorldPosition());

		objectInfoWindow_->SetVisible(true);
		objectInfoWindow_->SetPosition(position.x_, position.y_);
	}
}

void TGameSpaceState::HandleGameOver(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::START_MAIN_MENU,
		GameChangeStateEvent::P_DESTROY_PREVIOS_STATE, true);
}

void TGameSpaceState::SubscribeToEvents()
{
	SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(TGameSpaceState, HandlePhysicsPreStep));

	// Subscribe key down event
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(TGameSpaceState, HandleKeyDown));

	// Subscribe HandleUpdate() function for processing update events
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(TGameSpaceState, HandleUpdate));

	SubscribeToEvent(RE_INGAME_INFO_STRING, URHO3D_HANDLER(TGameSpaceState, HandleIngameInfo));
	SubscribeToEvent(menuButton_, E_PRESSED, URHO3D_HANDLER(TGameSpaceState, HandleMenuClick));

	SubscribeToEvent(E_MOUSEWHEEL, URHO3D_HANDLER(TGameSpaceState, HandleZoomCamera));

	// Subscribe HandlePostRenderUpdate() function for processing the post-render update event, during which we request debug geometry
	SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(TGameSpaceState, HandlePostRenderUpdate));

	SubscribeToEvent(RE_GAME_OVER, URHO3D_HANDLER(TGameSpaceState, HandleGameOver));
}

void TGameSpaceState::HandlePhysicsPreStep(StringHash eventType, VariantMap & eventData)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	UI* ui = GetSubsystem<UI>();
	Configuration* config = GetSubsystem<Configuration>();
	SharedData* shData = GetSubsystem<SharedData>();

	// Client: collect controls
	if (serverConnection)
	{
		Controls controls;

		// Only apply WASD controls if there is no focused UI element
		controls.Set(CTRL_FORWARD, config->GetActionKeyInput(Configuration::GameInputActions::MoveForward));
		controls.Set(CTRL_BACK, config->GetActionKeyInput(Configuration::GameInputActions::MoveBackward));
		controls.Set(CTRL_LEFT, config->GetActionKeyInput(Configuration::GameInputActions::MoveLeft));
		controls.Set(CTRL_RIGHT, config->GetActionKeyInput(Configuration::GameInputActions::MoveRight));

		controls.Set(THIRD_WEAPON_FIRE, config->GetActionKeyInput(Configuration::GameInputActions::FireThird));
		controls.Set(ULTIMATE_WEAPON_FIRE, config->GetActionKeyInput(Configuration::GameInputActions::FireUltimate));

		if (!ui->GetFocusElement())
		{
			controls.Set(MOUSE_LEFT_FIRE, config->GetActionKeyInput(Configuration::GameInputActions::FirePrimary));
			controls.Set(MOUSE_RIGHT_FIRE, config->GetActionKeyInput(Configuration::GameInputActions::FireSecondary));

			controls.extraData_["MousePos"] = GeneralMethods::GetMouseWorldPositionXY(camera_);
		}

		serverConnection->SetControls(controls);
	}
	else
	{
		// remove broken connections, TODO: remove also players from galaxy
		PlayersManager* playersManager = GetSubsystem<PlayersManager>();
		if (playersManager)
		{
			for (auto connectedPlayer : playersManager->connectedPlayers_)
			{
				Connection* connection = connectedPlayer.second_.connection;
				Node* playerNode = GeneralMethods::GetClientNode(scene_, connectedPlayer.second_.clientId, connectedPlayer.second_.teamId);
				if (connection && connection->IsConnected() && playerNode)
				{
					TControlObject* controlObject = playerNode->GetComponent<TControlObject>();
					// Get the last controls sent by the client
					const Controls& controls = connection->GetControls();
					if (controlObject)
						controlObject->ProcessControls(controls);
				}
			}
		}

		if (!shData->isServerWithoutPlayer_)
		{
			/// server player processing
			Controls controls;
			controls.Set(CTRL_FORWARD, config->GetActionKeyInput(Configuration::GameInputActions::MoveForward));
			controls.Set(CTRL_BACK, config->GetActionKeyInput(Configuration::GameInputActions::MoveBackward));
			controls.Set(CTRL_LEFT, config->GetActionKeyInput(Configuration::GameInputActions::MoveLeft));
			controls.Set(CTRL_RIGHT, config->GetActionKeyInput(Configuration::GameInputActions::MoveRight));

			controls.Set(THIRD_WEAPON_FIRE, config->GetActionKeyInput(Configuration::GameInputActions::FireThird));
			controls.Set(ULTIMATE_WEAPON_FIRE, config->GetActionKeyInput(Configuration::GameInputActions::FireUltimate));

			if (!ui->GetFocusElement())
			{
				controls.Set(MOUSE_LEFT_FIRE, config->GetActionKeyInput(Configuration::GameInputActions::FirePrimary));
				controls.Set(MOUSE_RIGHT_FIRE, config->GetActionKeyInput(Configuration::GameInputActions::FireSecondary));

				controls.extraData_["MousePos"] = GeneralMethods::GetMouseWorldPositionXY(camera_);
			}

			Node* controlNode = GeneralMethods::GetMyControlNode(scene_);
			if (!controlNode)
				return;

			TControlObject* controlObject = controlNode->GetComponent<TControlObject>();
			if (controlObject)
				controlObject->ProcessControls(controls);
		}
		else if (shData->isServerWithoutPlayer_)
		{
			/// server player processing
			Controls controls;
			controls.Set(CTRL_FORWARD, config->GetActionKeyInput(Configuration::GameInputActions::MoveForward));
			controls.Set(CTRL_BACK, config->GetActionKeyInput(Configuration::GameInputActions::MoveBackward));
			controls.Set(CTRL_LEFT, config->GetActionKeyInput(Configuration::GameInputActions::MoveLeft));
			controls.Set(CTRL_RIGHT, config->GetActionKeyInput(Configuration::GameInputActions::MoveRight));

			if (camera_)
			{
				Node* cameraNode = camera_->GetNode();
				
				Vector2 totalVector = Vector2::ZERO;
				if (controls.buttons_ & CTRL_FORWARD)
				{
					totalVector += Vector2::UP;
				}
				if (controls.buttons_ & CTRL_BACK)
				{
					totalVector += Vector2::DOWN;
				}
				if (controls.buttons_ & CTRL_LEFT)
				{
					totalVector += Vector2::LEFT;
				}
				if (controls.buttons_ & CTRL_RIGHT)
				{
					totalVector += Vector2::RIGHT;
				}

				cameraNode->Translate2D(totalVector * 1.5f);
			}
		}
	}
}

void TGameSpaceState::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
	using namespace KeyDown;

	S32 key = eventData[P_KEY].GetInt();

	// Close console (if open) or exit when ESC is pressed
	if (key == KEY_ESCAPE)
		EnterMenu();

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

#ifdef _DEBUG
	if (key == KEY_H)
	{
		Quaternion rot = camera_->GetNode()->GetWorldRotation();
		if (rot.w_ == 1)
		{
			camera_->GetNode()->SetWorldRotation(Quaternion(90, 0, 0));
			dontAutoRotateCamera_ = true;
		}
		else
		{
			camera_->GetNode()->SetWorldRotation(Quaternion(0, 0, 0));
			dontAutoRotateCamera_ = false;
		}
	}
#endif // _DEBUG
}

void TGameSpaceState::HandleIngameInfo(StringHash eventType, VariantMap& eventData)
{
	String gameInfoString = eventData[GameInfo::INFO_STRING].GetString();
	gameInfoText_->SetText(gameInfoString);
	gameInfoText_->SetVisible(true);
	gameInfoTimer_.Reset();
}

void TGameSpaceState::HandleZoomCamera(StringHash eventType, VariantMap& eventData)
{
	if (!camera_)
		return;

	Input* input = GetSubsystem<Input>();

	F32 zoom = 1.0f + Sign(input->GetMouseMoveWheel()) * 0.05f;
	camera_->SetZoom(camera_->GetZoom() * zoom);
}

void TGameSpaceState::HandleMenuClick(StringHash eventType, VariantMap & eventData)
{
	EnterMenu();
}

void TGameSpaceState::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
	PhysicsWorld2D* physicsWorld = scene_->GetComponent<PhysicsWorld2D>();
	if (drawDebug_)
	{
		physicsWorld->DrawDebugGeometry();
		NavigationMesh* navMesh = scene_->GetComponent<NavigationMesh>();
		if (navMesh)
		{
			navMesh->DrawDebugGeometry(false);
		}
	}
}

void TGameSpaceState::HandleClientSceneLoaded(StringHash eventType, VariantMap & eventData)
{
	Connection* clientConnection = static_cast<Connection*>(eventData[ClientSceneLoaded::P_CONNECTION].GetPtr());
	clientConnection->SendRemoteEvent(RE_GAME_STARTED, true);
}

void TGameSpaceState::HandlePlayerRemoved(StringHash eventType, VariantMap & eventData)
{
	U32 clientId = eventData[GalaxyRemovePlayer::CLIENT_ID].GetUInt();
	U32 teamNumber = eventData[GalaxyRemovePlayer::TEAM_NUMBER].GetUInt();

	Node* playerNode = GeneralMethods::GetClientNode(scene_, clientId, teamNumber);
	if (playerNode)
	{
		playerNode->Remove();
	}
}
