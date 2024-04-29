#pragma once

#include "stateManager/gameStates.h"
#include "legacyMode/galaxy/galaxy.h"

namespace Urho3D
{
	class Button;
	class Text;
	class ListView;
	class DropDownList;
	class Slider;
}

using namespace Urho3D;

class GameMultiGalaxyState : public IGameState
{
	URHO3D_OBJECT(GameMultiGalaxyState, IGameState);

public:

	GameMultiGalaxyState(Urho3D::Context * context);
	virtual ~GameMultiGalaxyState();

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:
	Races::Races race_;
	WeakPtr<ListView> playersList_;
	WeakPtr<DropDownList> teamList_;

	U32     minNumOfStars_;
	U32     minNumOfEnemies_;

	U32     maxNumOfStars_;
	U32     maxNumOfEnemies_;

	WeakPtr<DropDownList>  galaxyTypeList_;
	WeakPtr<DropDownList>  galaxyAgeList_;

	WeakPtr<Text>          starsCountTitle_;
	WeakPtr<Text>          enemiesCountTitle_;

	WeakPtr<Slider>        starsSlider_;
	WeakPtr<Slider>        enemiesSlider_;

	WeakPtr<Button>        returnToMenu_;
	WeakPtr<Button>        startGame_;

	void HandleRaceSelection(StringHash eventType, VariantMap& eventData);

	void HandleGalaxyTypeChange(StringHash eventType, VariantMap& eventData);
	void HandleGalaxyAgeChange(StringHash eventType, VariantMap& eventData);

	void HandleStarsSliderChange(StringHash eventType, VariantMap& eventData);
	void HandleEnemiesSliderChange(StringHash eventType, VariantMap& eventData);

	void MenuButtonClick(StringHash eventType, VariantMap& eventData);
	void StartGameButtonClick(StringHash eventType, VariantMap& eventData);

	U32             myTeam_;
	Galaxy::Type    type_;
	Galaxy::Age     age_;
	U32             numberOfStars_;
	U32             numberOfEnemies_;

	void HandleClientListNewPlayer(StringHash eventType, VariantMap& eventData);
	void HandleClientListRemovePlayer(StringHash eventType, VariantMap& eventData);
	void HandleClientListPlayerTeamChange(StringHash eventType, VariantMap& eventData);
	void HandleClientGameParametersChange(StringHash eventType, VariantMap& eventData);

	void HandleClientSideTeamChange(StringHash eventType, VariantMap& eventData);

	void HandleGameStarted(StringHash eventType, VariantMap& eventData);
	void HandleServerStopped(StringHash eventType, VariantMap& eventData);

	void InsertPlayerToListView(U32 clientId, U32 teamId, bool isYou);
	void RemovePlayerFromListView(U32 clientId);

	VariantMap ComposeGameParameters() const;
};
