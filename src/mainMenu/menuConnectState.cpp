#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Network/Network.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "config.h"

#include "mainMenu/menuConnectState.h"

using namespace Urho3D;

GameMenuConnectState::GameMenuConnectState(Urho3D::Context * context) :
	IGameState(context)
{
}

void GameMenuConnectState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/menuConnect.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	// menu buttons
	connectionText_ = static_cast<LineEdit*>(uiStateRoot_->GetChild("connectionText_", true));
	connect_ =        static_cast<Button*>(uiStateRoot_->GetChild("connect_", true));
	back_ =           static_cast<Button*>(uiStateRoot_->GetChild("back_", true));

	connectionText_->SetText("localhost");
}

void GameMenuConnectState::Enter()
{
	uiStateRoot_->SetVisible(true);
	uiStateRoot_->UpdateLayout();

	SubscribeToEvent(connect_, E_PRESSED, URHO3D_HANDLER(GameMenuConnectState, HandleConnectClick));
	SubscribeToEvent(back_, E_PRESSED, URHO3D_HANDLER(GameMenuConnectState, HandleBackClick));
}

void GameMenuConnectState::HandleConnectClick(StringHash eventType, VariantMap & eventData)
{
	Network* network = GetSubsystem<Network>();
	Configuration* config = GetSubsystem<Configuration>();

	String address = connectionText_->GetText();
	if (address.Empty())
		address = config->GetValue("address").GetString();
	else
	{
		config->SetValue("address", address);
		config->Save();
	}

	bool result = network->Connect(address, config->GetValue("port").GetInt(), nullptr);
	if (result)
	{
		SendEvent(G_STATE_CHANGE,
			GameChangeStateEvent::P_STATE, GameStates::MULTI_GALAXY_SETUP);
	}
}

void GameMenuConnectState::HandleBackClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::MENU_MULTI_GAME);
}

void GameMenuConnectState::Exit()
{
	uiStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void GameMenuConnectState::Pause()
{
}

void GameMenuConnectState::Resume()
{
}
