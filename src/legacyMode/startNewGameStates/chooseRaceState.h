#pragma once

#include "stateManager/gameStates.h"
#include "races/races.h"

namespace Urho3D
{
	class ListView;
	class Button;
	class Sprite;
	class Text;
	class Texture2D;
}

using namespace Urho3D;

class GameChooseRaceState : public IGameState
{
	URHO3D_OBJECT(GameChooseRaceState, IGameState);

public:

	enum class Side
	{
		Left,
		Right
	};

	GameChooseRaceState(Urho3D::Context * context);
	virtual ~GameChooseRaceState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:
	
	Races::Races race_;
	U32 shipNum_;

	void HandleRaceSelection(StringHash eventType, VariantMap& eventData);

	void MenuButtonClick(StringHash eventType, VariantMap& eventData);
	void NextButtonClick(StringHash eventType, VariantMap& eventData);

	void LeftButtonClick(StringHash eventType, VariantMap& eventData);
	void RightButtonClick(StringHash eventType, VariantMap& eventData);

	void LeftWeaponClick(StringHash eventType, VariantMap& eventData);
	void RightWeaponClick(StringHash eventType, VariantMap& eventData);
	void EngineClick(StringHash eventType, VariantMap& eventData);

	void ShiftShip(Side side);

	Vector<Texture2D*>  racesTextures_;
	Vector<Texture2D*>  shipsTextures_;

	WeakPtr<ListView>   raceList_;
	WeakPtr<Button>     leftChooseButton_;
	WeakPtr<Button>     rightChooseButton_;
	WeakPtr<Sprite>     shipImage_;

	WeakPtr<Button>     leftWeapon_;
	WeakPtr<Button>     rightWeapon_;
	WeakPtr<Button>     engine_;
	WeakPtr<Button>     hyperDrive_;

	WeakPtr<Text>		leftWeaponName_;
	WeakPtr<Text>		rightWeaponName_;
	WeakPtr<Text>		engineName_;

	WeakPtr<Button>     returnToMenu_;
	WeakPtr<Button>     nextStage_;

	HashMap<String, Node*> weapons_;
	HashMap<String, Node*> engines_;
};
