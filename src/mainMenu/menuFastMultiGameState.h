#pragma once

#include "stateManager/gameStates.h"
#include "utility/simpleTypes.h"

namespace Urho3D
{
	class Button;
	class Text;
	class ListView;
	class DropDownList;
	class Slider;
}

using namespace Urho3D;

class MenuFastMultiGameState : public IGameState
{
	URHO3D_OBJECT(MenuFastMultiGameState, IGameState);

public:

	MenuFastMultiGameState(Urho3D::Context * context);
	virtual ~MenuFastMultiGameState();

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:
	Timer gameStartupTimer_;

	WeakPtr<ListView> playersList_;
	WeakPtr<DropDownList> teamList_;

	WeakPtr<Button> returnToMenu_;
	WeakPtr<Button> startGame_;

	void MenuButtonClick(StringHash eventType, VariantMap& eventData);
	void StartGameButtonClick(StringHash eventType, VariantMap& eventData);

	void StartGame();

	U32 myTeam_;

	void HandleClientListNewPlayer(StringHash eventType, VariantMap& eventData);
	void HandleClientListRemovePlayer(StringHash eventType, VariantMap& eventData);
	void HandleClientListPlayerTeamChange(StringHash eventType, VariantMap& eventData);

	void HandleClientSideTeamChange(StringHash eventType, VariantMap& eventData);

	void HandleGameStarted(StringHash eventType, VariantMap& eventData);
	void HandleServerStopped(StringHash eventType, VariantMap& eventData);

	void InsertPlayerToListView(U32 clientId, U32 teamId, bool isYou);
	void RemovePlayerFromListView(U32 clientId);

	void HandleTimeUpdate(StringHash eventType, VariantMap& eventData);
};
