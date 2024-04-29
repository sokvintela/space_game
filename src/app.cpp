#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/IO/FileSystem.h>
#ifdef URHO3D_LUA
#include <Urho3D/LuaScript/LuaScript.h>
#endif

#include "config.h"
#include "stateManager/gameStateHandler.h"
#include "stateManager/gameStateEvents.h"
#include "utility/sharedData.h"
#include "legacyMode/utility/lSharedData.h"
#include "prefabs/prefabsStorage.h"
#include "legacyMode/space/lSpaceState.h"
#include "tacticalMode/space/tSpaceState.h"
#include "ai/AIManager.h"
#include "utility/registration.h"
#include "legacyMode/utility/lRegistration.h"

#include "app.h"

using namespace Urho3D;

URHO3D_DEFINE_APPLICATION_MAIN(SpaceGame)

#ifdef URHO3D_LUA
extern S32 tolua_gameStateHandler_open(lua_State*);
#endif

SpaceGame::SpaceGame(Context* context) :
	Application(context)
{
	scriptedRun_ = false;
}

void SpaceGame::Setup()
{
	Configuration* config = new Configuration(context_);
	context_->RegisterSubsystem(config);
	config->Load();

	// Modify engine startup parameters
	engineParameters_["WindowTitle"] = GenerateWindowTitle();
	engineParameters_["WindowIcon"] = "Textures/appIcon.png";
	engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir(GetTypeName(), "logs") + GetTypeName() + ".log";
	engineParameters_["FullScreen"] = config->GetValue("fullscreen").GetBool();
	engineParameters_["Borderless"] = config->GetValue("borderless").GetBool();
	engineParameters_["WindowWidth"] = config->GetValue("width").GetInt();
	engineParameters_["WindowHeight"] = config->GetValue("height").GetInt();
	engineParameters_["Sound"] = true;

	bool serverWithoutPlayer = false;
	const Vector<String>& arguments = GetArguments();
	for (auto argument : arguments)
	{
		if (argument == "-h")
		{
			engineParameters_["Headless"] = true;
		}
		else if (argument == "-s")
		{
			serverWithoutPlayer = true;
		}
#ifdef URHO3D_LUA
		else
		{
			scriptFileName_ = GetInternalPath(argument);
			String extension = GetExtension(scriptFileName_);

			if (extension == ".lua" || extension == ".luc")
				scriptedRun_ = true;
		}
#endif // URHO3D_LUA
	}

#ifdef URHO3D_LUA
	if (scriptFileName_ == String::EMPTY)
	{
		scriptFileName_ = "Scripts/start.lua";
		scriptedRun_ = true;
	}
#endif // URHO3D_LUA

	LSharedData* lSharedData = new LSharedData(context_);
	SharedData* sharedData = new SharedData(context_);

	context_->RegisterSubsystem(lSharedData);
	context_->RegisterSubsystem(sharedData);

	lSharedData->isServerWithoutPlayer_ = serverWithoutPlayer;
	sharedData->isServerWithoutPlayer_ = serverWithoutPlayer;

	LGameSpaceState::RegisterComponents(context_);
}

void SpaceGame::Start()
{
	// set the mouse visible
	GetSubsystem<Input>()->SetMouseVisible(true);

	// Get default style
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

	// Initialize localization
	Localization* l10n = GetSubsystem<Localization>();
	l10n->LoadJSONFile("Locale/StringsEnRu.json");

	Configuration* config = GetSubsystem<Configuration>();
	l10n->SetLanguage(config->GetValue("lang").GetString());

	Audio* audio = GetSubsystem<Audio>();
	if (audio)
	{
		audio->SetMasterGain("effects", config->GetValue("sound").GetFloat());
	}

	// Create console
	Console* console = engine_->CreateConsole();
	if (console)
	{
		console->SetDefaultStyle(xmlFile);
		console->GetBackground()->SetOpacity(0.8f);
	}

	// Create debug HUD.
	DebugHud* debugHud = engine_->CreateDebugHud();
	if (debugHud)
	{
		debugHud->SetDefaultStyle(xmlFile);
	}

	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(SpaceGame, HandleKeyDown));
	SubscribeToEvent(G_STATE_EXIT_GAME, URHO3D_HANDLER(SpaceGame, HandleGameExit));

	Registration::RegisterGameObjects(context_);
	LRegistration::RegisterGameObjects(context_);

	AI::AIManager* AIManager = new AI::AIManager(context_);
	context_->RegisterSubsystem(AIManager);
	AIManager->LoadBehaviors();

	context_->RegisterSubsystem(new PrefabsStorage(context_));

	GameStateHandler* stateHandler = new GameStateHandler(context_);
	context_->RegisterSubsystem(stateHandler);

	if (!scriptedRun_)
	{
		if (GetSubsystem<SharedData>()->isServerWithoutPlayer_)
			stateHandler->Start(GameStates::MULTI_FAST);
		else
			stateHandler->Start(GameStates::START_MAIN_MENU);
	}
	else
	{
#ifdef URHO3D_LUA
		// Instantiate and register the Lua script subsystem
		LuaScript* luaScript = new LuaScript(context_);
		context_->RegisterSubsystem(luaScript);

		lua_State* luaState = luaScript->GetState();
		tolua_gameStateHandler_open(luaState);

		// If script loading is successful, proceed to main loop
		if (luaScript->ExecuteFile(scriptFileName_))
		{
			luaScript->ExecuteFunction("Start");
		}
#endif // URHO3D_LUA
	}
}

void SpaceGame::Stop()
{
	engine_->DumpResources(true);
}

String SpaceGame::GenerateWindowTitle() const
{
	String result =
		GetTypeName()
		+ " (" + GetPlatform() + ")"
#ifdef _WIN64
		+ " (x64)"
#else
		+ " (x86)"
#endif // _WIN64
#ifdef _DEBUG
		+ " (Debug)"
#endif // _DEBUG
		;

	return result;
}

void SpaceGame::HandleKeyDown(StringHash eventType, VariantMap & eventData)
{
	S32 qual = eventData[KeyDown::P_QUALIFIERS].GetInt();
	S32 key = eventData[KeyDown::P_KEY].GetInt();

	// ctrl + F1 toggles console
	if (key == KEY_F1 && (qual & QUAL_CTRL))
	{
		Console* console = GetSubsystem<Console>();
		console->SetVisible(!console->IsVisible());
	}

	if (qual & QUAL_CTRL)
	{
		DebugHud* debugHud = GetSubsystem<DebugHud>();
		switch (key)
		{
			case KEY_1:
				debugHud->Toggle(DEBUGHUD_SHOW_STATS);
				break;
			case KEY_2:
				debugHud->Toggle(DEBUGHUD_SHOW_MODE);
				break;
			case KEY_3:
				debugHud->Toggle(DEBUGHUD_SHOW_PROFILER);
				break;
			case KEY_4:
				debugHud->Toggle(DEBUGHUD_SHOW_MEMORY);
				break;
			case KEY_5:
				debugHud->Toggle(DEBUGHUD_SHOW_EVENTPROFILER);
				break;
		}
	}
}

void SpaceGame::HandleGameExit(StringHash eventType, VariantMap& eventData)
{
	engine_->Exit();
}
