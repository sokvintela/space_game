#include <Urho3D/Core/Object.h>

#include "utility/simpleTypes.h"
#include "stateManager/statesList.h"

#include <memory>
#include <map>

using namespace Urho3D;

class IGameState;

class URHO3D_API GameStateHandler : public Object
{
	URHO3D_OBJECT(GameStateHandler, Object);
public:
	GameStates::GameState currentState_;

	GameStateHandler(Urho3D::Context* context);
	~GameStateHandler();

	void Start(U32 state);
	void PushState(U32 state);
	void PopState(U32 state);
	void EnterState(U32 state);
	void ExitState(U32 state);
	void Pause();
	void Resume();
	void RemoveAllStates();
	void EnterSingleState(U32 state);

private:
	std::unique_ptr<IGameState> CreateState(U32 state);

	// handler events must be public
	void ChangedStateEvent(StringHash eventType, VariantMap& eventData); 

	std::map<U32, std::unique_ptr<IGameState>> states_;
};
