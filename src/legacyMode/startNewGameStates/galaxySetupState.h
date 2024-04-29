#pragma once

#include "stateManager/gameStates.h"
#include "legacyMode/galaxy/galaxy.h"

namespace Urho3D
{
	class Button;
	class Text;
	class ListView;
	class Slider;
}

using namespace Urho3D;

class GameGalaxySetupState : public IGameState
{
	URHO3D_OBJECT(GameGalaxySetupState, IGameState);

public:

	enum class Side
	{
		Left,
		Right
	};

	GameGalaxySetupState(Urho3D::Context * context);
	virtual ~GameGalaxySetupState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:

	S32		minNumOfStars_;
	S32		minNumOfEnemies_;

	S32		maxNumOfStars_;
	S32		maxNumOfEnemies_;

	WeakPtr<ListView>	galaxyTypeList_;
	WeakPtr<ListView>	galaxyAgeList_;

	WeakPtr<Text>		starsCountTitle_;
	WeakPtr<Text>		enemiesCountTitle_;

	WeakPtr<Slider>		starsSlider_;
	WeakPtr<Slider>		enemiesSlider_;

	WeakPtr<Button>		returnToMenu_;
	WeakPtr<Button>		nextStage_;

	void HandleGalaxyTypeChange(StringHash eventType, VariantMap& eventData);
	void HandleGalaxyAgeChange(StringHash eventType, VariantMap& eventData);

	void HandleStarsSliderChange(StringHash eventType, VariantMap& eventData);
	void HandleEnemiesSliderChange(StringHash eventType, VariantMap& eventData);

	void MenuButtonClick(StringHash eventType, VariantMap& eventData);
	void NextButtonClick(StringHash eventType, VariantMap& eventData);

	Galaxy::Type	type_;
	Galaxy::Age		age_;
	S32				numberOfStars_;
	S32				numberOfEnemies_;
};
