#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/Slider.h>

#include "stateManager/statesList.h"
#include "stateManager/gameStateEvents.h"
#include "legacyMode/utility/lSharedData.h"

#include "legacyMode/startNewGameStates/galaxySetupState.h"

using namespace Urho3D;

GameGalaxySetupState::GameGalaxySetupState(Urho3D::Context * context) :
	IGameState(context),
	minNumOfStars_(2),
	minNumOfEnemies_(1),
	maxNumOfStars_(100),
	maxNumOfEnemies_(100),
	numberOfStars_(15),
	numberOfEnemies_(7),
	type_(Galaxy::Type::Elliptic),
	age_(Galaxy::Age::Medium)
{
}

void GameGalaxySetupState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/galaxySetup.xml");
	uiStateRoot_->LoadXML(layout->GetRoot(), style);

	galaxyTypeList_ =		static_cast<ListView*>(uiStateRoot_->GetChild("galaxyTypeList_", true));
	galaxyAgeList_ =		static_cast<ListView*>(uiStateRoot_->GetChild("galaxyAgeList_", true));

	starsCountTitle_ =		static_cast<Text*>(uiStateRoot_->GetChild("starsCountTitle_", true));
	enemiesCountTitle_ =	static_cast<Text*>(uiStateRoot_->GetChild("enemiesCountTitle_", true));

	starsSlider_ =			static_cast<Slider*>(uiStateRoot_->GetChild("starsSlider_", true));
	enemiesSlider_ =		static_cast<Slider*>(uiStateRoot_->GetChild("enemiesSlider_", true));

	returnToMenu_ =			static_cast<Button*>(uiStateRoot_->GetChild("returnToMenu_", true));
	nextStage_ =			static_cast<Button*>(uiStateRoot_->GetChild("nextStage_", true));
}

void GameGalaxySetupState::Enter()
{
	uiStateRoot_->SetVisible(true);

	SubscribeToEvent(galaxyTypeList_, E_ITEMSELECTED, URHO3D_HANDLER(GameGalaxySetupState, HandleGalaxyTypeChange));
	SubscribeToEvent(galaxyAgeList_, E_ITEMSELECTED, URHO3D_HANDLER(GameGalaxySetupState, HandleGalaxyAgeChange));

	SubscribeToEvent(starsSlider_, E_SLIDERCHANGED, URHO3D_HANDLER(GameGalaxySetupState, HandleStarsSliderChange));
	SubscribeToEvent(enemiesSlider_, E_SLIDERCHANGED, URHO3D_HANDLER(GameGalaxySetupState, HandleEnemiesSliderChange));

	SubscribeToEvent(returnToMenu_, E_PRESSED, URHO3D_HANDLER(GameGalaxySetupState, MenuButtonClick));
	SubscribeToEvent(nextStage_, E_PRESSED, URHO3D_HANDLER(GameGalaxySetupState, NextButtonClick));
}

void GameGalaxySetupState::HandleGalaxyTypeChange(StringHash eventType, VariantMap & eventData)
{
	type_ = static_cast<Galaxy::Type>(eventData[ItemSelected::P_SELECTION].GetInt());
}

void GameGalaxySetupState::HandleGalaxyAgeChange(StringHash eventType, VariantMap & eventData)
{
	age_ = static_cast<Galaxy::Age>(eventData[ItemSelected::P_SELECTION].GetInt());
}

void GameGalaxySetupState::HandleStarsSliderChange(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;

	numberOfStars_ = eventData[P_VALUE].GetInt() + minNumOfStars_;
	starsCountTitle_->SetText(String(numberOfStars_));
}

void GameGalaxySetupState::HandleEnemiesSliderChange(StringHash eventType, VariantMap& eventData)
{
	using namespace SliderChanged;

	numberOfEnemies_ = eventData[P_VALUE].GetInt() + minNumOfEnemies_;
	enemiesCountTitle_->SetText(String(numberOfEnemies_));
}

void GameGalaxySetupState::MenuButtonClick(StringHash eventType, VariantMap & eventData)
{
	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::CHOOSE_RACE);
}

void GameGalaxySetupState::NextButtonClick(StringHash eventType, VariantMap & eventData)
{
	LSharedData* sharedData = GetSubsystem<LSharedData>();
	sharedData->isFastGame_ = false;

	sharedData->type_ = type_;
	sharedData->age_ = age_;
	sharedData->numberOfStars_ = numberOfStars_;
	sharedData->numberOfEnemies_ = numberOfEnemies_;

	SendEvent(G_STATE_CHANGE,
		GameChangeStateEvent::P_STATE, GameStates::LSPACE);
}

void GameGalaxySetupState::Exit()
{
	uiStateRoot_->SetVisible(false);
	UnsubscribeFromAllEvents();
}

void GameGalaxySetupState::Pause()
{
}

void GameGalaxySetupState::Resume()
{
}
