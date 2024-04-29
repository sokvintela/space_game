#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/IO/Log.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "legacyMode/utility/lSharedData.h"
#include "prefabs/prefabsStorage.h"
#include "legacyMode/utility/lGeneralMethods.h"

#include "legacyMode/startNewGameStates/chooseRaceState.h"

const String race_image[static_cast<U32>(Races::Races::R_Count)] =
{
	"Urho2D/civilisations/Irrs.png",
	"Urho2D/civilisations/Urrs.png",
	"Urho2D/civilisations/Onns.png",
	"Urho2D/civilisations/Tirrs.png",
	"Urho2D/civilisations/Chinchi.png",
	"Urho2D/civilisations/Random.png"
};

static const U32 shipCount = 2;

const String ship_image[shipCount] =
{
	"Urho2D/spaceships/shipConfig1.png",
	"Urho2D/civilisations/Random.png"
};

using namespace Urho3D;

GameChooseRaceState::GameChooseRaceState(Urho3D::Context * context) :
	IGameState(context),
	race_(Races::Races::R_Irrs),
	shipNum_(0)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	racesTextures_.Resize(static_cast<U32>(Races::Races::R_Count));
	shipsTextures_.Resize(static_cast<U32>(shipCount));

	for (U32 i = 0; i < static_cast<U32>(Races::Races::R_Count); i++)
	{
		racesTextures_[i] = cache->GetResource<Texture2D>(race_image[i]);
	}

	for (U32 i = 0; i < static_cast<U32>(shipsTextures_.Size()); i++)
	{
		shipsTextures_[i] = cache->GetResource<Texture2D>(ship_image[i]);
	}
}

void GameChooseRaceState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/chooseRace.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	raceList_ =          static_cast<ListView*>(uiStateRoot_->GetChild("raceList_", true));
	leftChooseButton_ =  static_cast<Button*>(uiStateRoot_->GetChild("leftChooseButton_", true));
	shipImage_ =         static_cast<Sprite*>(uiStateRoot_->GetChild("shipImage_", true));
	rightChooseButton_ = static_cast<Button*>(uiStateRoot_->GetChild("rightChooseButton_", true));
	returnToMenu_ =      static_cast<Button*>(uiStateRoot_->GetChild("returnToMenu_", true));
	nextStage_ =         static_cast<Button*>(uiStateRoot_->GetChild("nextStage_", true));

	UIElement* eqiupmentParent = shipImage_->GetParent();

	UIElement* leftWeaponCell = eqiupmentParent->CreateChild<UIElement>("leftWeapon");
	leftWeaponCell->SetLayout(LM_VERTICAL, 3);
	leftWeaponCell->SetStyleAuto();
	leftWeaponCell->SetPosition(50, 50);

	leftWeapon_ = leftWeaponCell->CreateChild<Button>();
	leftWeapon_->SetFixedSize(50, 50);

	leftWeaponName_ = leftWeaponCell->CreateChild<Text>();
	leftWeaponName_->SetStyleAuto();
	leftWeaponName_->SetColor(Color::BLACK);
	
	UIElement* rightWeaponCell = eqiupmentParent->CreateChild<UIElement>("rightWeapon");
	rightWeaponCell->SetLayout(LM_VERTICAL, 3);
	rightWeaponCell->SetStyleAuto();
	rightWeaponCell->SetPosition(300, 50);

	rightWeapon_ = rightWeaponCell->CreateChild<Button>();
	rightWeapon_->SetFixedSize(50, 50);

	rightWeaponName_ = rightWeaponCell->CreateChild<Text>();
	rightWeaponName_->SetStyleAuto();
	rightWeaponName_->SetColor(Color::BLACK);

	UIElement* engineCell = eqiupmentParent->CreateChild<UIElement>("engine");
	engineCell->SetLayout(LM_VERTICAL, 3);
	engineCell->SetStyleAuto();
	engineCell->SetPosition(50, 300);

	engine_ = engineCell->CreateChild<Button>();
	engine_->SetFixedSize(50, 50);

	engineName_ = engineCell->CreateChild<Text>();
	engineName_->SetStyleAuto();
	engineName_->SetColor(Color::BLACK);

	PrefabsStorage* prefabs = GetSubsystem<PrefabsStorage>();

	weapons_ = prefabs->GetAvailabeLegacyEquipment(LEquipment::ET_Weapon, nullptr);
	if (weapons_.Empty())
	{
		URHO3D_LOGERROR("No weapons available");
	}
	else
	{
		leftWeapon_->SetVar("currentWeapon", weapons_.Begin()->first_);
		rightWeapon_->SetVar("currentWeapon", weapons_.Begin()->first_);
	}

	engines_ = prefabs->GetAvailabeLegacyEquipment(LEquipment::ET_Engine, nullptr);
	if (engines_.Empty())
	{
		URHO3D_LOGERROR("No engines available");
	}
	else
	{
		engine_->SetVar("currentEngine", engines_.Begin()->first_);
	}

	// TODO: race traits texts
}

void GameChooseRaceState::Enter()
{
	uiStateRoot_->SetVisible(true);

	SubscribeToEvent(raceList_, E_ITEMSELECTED, URHO3D_HANDLER(GameChooseRaceState, HandleRaceSelection));

	SubscribeToEvent(leftChooseButton_, E_PRESSED, URHO3D_HANDLER(GameChooseRaceState, LeftButtonClick));
	SubscribeToEvent(rightChooseButton_, E_PRESSED, URHO3D_HANDLER(GameChooseRaceState, RightButtonClick));

	SubscribeToEvent(returnToMenu_, E_PRESSED, URHO3D_HANDLER(GameChooseRaceState, MenuButtonClick));
	SubscribeToEvent(nextStage_, E_PRESSED, URHO3D_HANDLER(GameChooseRaceState, NextButtonClick));

	SubscribeToEvent(leftWeapon_, E_PRESSED, URHO3D_HANDLER(GameChooseRaceState, LeftWeaponClick));
	SubscribeToEvent(rightWeapon_, E_PRESSED, URHO3D_HANDLER(GameChooseRaceState, RightWeaponClick));
	SubscribeToEvent(engine_, E_PRESSED, URHO3D_HANDLER(GameChooseRaceState, EngineClick));

	String currentLeftWeaponType = leftWeapon_->GetVar("currentWeapon").GetString();
	auto leftIt = weapons_.Find(currentLeftWeaponType);
	if (leftIt != weapons_.End())
	{
		leftWeapon_->SetTexture(LGeneralMethods::GetTextureFromNode(leftIt->second_));
		leftWeaponName_->SetText(leftIt->first_);
	}

	String currentRightWeaponType = rightWeapon_->GetVar("currentWeapon").GetString();
	auto rightIt = weapons_.Find(currentRightWeaponType);
	if (rightIt != weapons_.End())
	{
		rightWeapon_->SetTexture(LGeneralMethods::GetTextureFromNode(rightIt->second_));
		rightWeaponName_->SetText(rightIt->first_);
	}

	String currentEngineType = engine_->GetVar("currentEngine").GetString();
	auto engineIt = engines_.Find(currentEngineType);
	if (engineIt != engines_.End())
	{
		engine_->SetTexture(LGeneralMethods::GetTextureFromNode(engineIt->second_));
		engineName_->SetText(engineIt->first_);
	}
}

void GameChooseRaceState::LeftButtonClick(StringHash eventType, VariantMap & eventData)
{
	ShiftShip(Side::Left);
}

void GameChooseRaceState::RightButtonClick(StringHash eventType, VariantMap & eventData)
{
	ShiftShip(Side::Right);
}

void GameChooseRaceState::LeftWeaponClick(StringHash eventType, VariantMap & eventData)
{
	if (weapons_.Empty())
		return;

	String currentType = leftWeapon_->GetVar("currentWeapon").GetString();
	auto it = weapons_.Find(currentType);
	it++;
	if (it == weapons_.End())
		it = weapons_.Begin();

	leftWeapon_->SetVar("currentWeapon", it->first_);
	leftWeapon_->SetTexture(LGeneralMethods::GetTextureFromNode(it->second_));
	leftWeaponName_->SetText(it->first_);
}

void GameChooseRaceState::RightWeaponClick(StringHash eventType, VariantMap & eventData)
{
	if (weapons_.Empty())
		return;

	String currentType = rightWeapon_->GetVar("currentWeapon").GetString();
	auto it = weapons_.Find(currentType);
	it++;
	if (it == weapons_.End())
		it = weapons_.Begin();

	rightWeapon_->SetVar("currentWeapon", it->first_);
	rightWeapon_->SetTexture(LGeneralMethods::GetTextureFromNode(it->second_));
	rightWeaponName_->SetText(it->first_);
}

void GameChooseRaceState::EngineClick(StringHash eventType, VariantMap & eventData)
{
	if (engines_.Empty())
		return;

	String currentType = engine_->GetVar("currentEngine").GetString();
	auto it = engines_.Find(currentType);
	it++;
	if (it == engines_.End())
		it = engines_.Begin();

	engine_->SetVar("currentEngine", it->first_);
	engine_->SetTexture(LGeneralMethods::GetTextureFromNode(it->second_));
	engineName_->SetText(it->first_);
}

void GameChooseRaceState::ShiftShip(Side side)
{
	if (side == Side::Left && shipNum_ > 0)
		shipNum_--;
	else if (side == Side::Right && shipNum_ < (shipCount - 1))
		shipNum_++;

	shipImage_->SetTexture(shipsTextures_[shipNum_]);
}

void GameChooseRaceState::HandleRaceSelection(StringHash eventType, VariantMap & eventData)
{
	int index = eventData[ItemSelected::P_SELECTION].GetInt();
	race_ = static_cast<Races::Races>(index);
}

void GameChooseRaceState::MenuButtonClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::START_MAIN_MENU);
}

void GameChooseRaceState::NextButtonClick(StringHash eventType, VariantMap & eventData)
{
	if (race_ == Races::Races::R_Random)
	{
		S32 raceNumber = Urho3D::Random(static_cast<S32>(Races::Races::R_Random));
		race_ = static_cast<Races::Races>(raceNumber);
	}

	LSharedData* sharedData = GetSubsystem<LSharedData>();
	sharedData->myRace_ = race_;
	sharedData->leftWeapon_ = leftWeapon_->GetVar("currentWeapon").GetString();
	sharedData->rightWeapon_ = rightWeapon_->GetVar("currentWeapon").GetString();
	sharedData->engine_ = engine_->GetVar("currentEngine").GetString();

	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::GALAXY_SETUP);
}

void GameChooseRaceState::Exit()
{
	uiStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void GameChooseRaceState::Pause()
{
}

void GameChooseRaceState::Resume()
{
}
