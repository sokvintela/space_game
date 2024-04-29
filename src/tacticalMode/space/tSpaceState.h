#pragma once

#include <Urho3D/Core/Timer.h>

#include "stateManager/gameStates.h"
#include "utility/simpleTypes.h"

using namespace Urho3D;

namespace Urho3D
{
	class Button;
	class Camera;
	class Sprite;
	class Text;
};

class TGameEventProcessing;
class TCollisionHandler;
class TShipEquipment;
class TStatusLineUI;
class TFindObjectsHelper;
class BaseShipUI;

class TGameSpaceState : public IGameState
{
	URHO3D_OBJECT(TGameSpaceState, IGameState);
public:

	static void RegisterEvents(Context* context);

	// game info text and timer to hide info
	WeakPtr<Text> gameInfoText_;
	Timer gameInfoTimer_;

	TGameSpaceState(Context * context);
	virtual ~TGameSpaceState();

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

	virtual void EnterMenu();
	virtual void ExitSubstate();

	void CreateGame();
	void LoadGame();
	void EnterGame();

	void CreateMenuSubstate();

	void SetupCamera();
	void SetupViewport() const;

	// Create logic
	void CreateBackground() const;
	void CreateStarSystem() const;
	void CreateUI();

	void BuildNavMesh() const;

	void HandleGameOver(StringHash eventType, VariantMap& eventData);

	void SubscribeToEvents();
	void HandlePhysicsPreStep(StringHash eventType, VariantMap& eventData);
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleIngameInfo(StringHash eventType, VariantMap& eventData);

	/// Handle game click
	void HandleMenuClick(StringHash eventType, VariantMap& eventData);

	void HandleZoomCamera(StringHash eventType, VariantMap& eventData);
	void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

	void HandleClientSceneLoaded(StringHash eventType, VariantMap & eventData);
	void HandlePlayerRemoved(StringHash eventType, VariantMap & eventData);

private:
	UniquePtr<TGameEventProcessing> gameEventProcessing_;
	UniquePtr<TCollisionHandler> collisionHandler_;
	
	UniquePtr<TShipEquipment> shipEquipment_;
	UniquePtr<TStatusLineUI> statusLineUI_;
	UniquePtr<TFindObjectsHelper> findObjectsHelper_;
	UniquePtr<BaseShipUI> baseShipUI_;

	WeakPtr<UIElement>	uiSpaceState_;
	ISubState*	menuSubState_;

	/// Camera scene node.
	WeakPtr<Camera> camera_;

	// buttons
	WeakPtr<Button>              menuButton_;

	// object info window
	WeakPtr<UIElement>           objectInfoWindow_;

	bool drawDebug_;
	bool dontAutoRotateCamera_ = false;
};
