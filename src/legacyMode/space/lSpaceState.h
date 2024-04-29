#pragma once

#include <Urho3D/Core/Timer.h>

#include "stateManager/gameStates.h"
#include "utility/simpleTypes.h"
#include "legacyMode/utility/lGeneralMethods.h"

using namespace Urho3D;

namespace Urho3D
{
	class Button;
	class Camera;
	class Sprite;
	class Text;
};

class LCollisionHandler;
class LShipEquipment;
class LSpaceBuildings;
class LKnowledgeUI;
class LStatusLineUI;
class LFindObjectsHelper;

class LGameSpaceState : public IGameState
{
	URHO3D_OBJECT(LGameSpaceState, IGameState);
public:

	enum class SubStates
	{
		Menu,
		SaveGame,
		Planet,
		Map,
		ShipInfo,
		Count
	} subState_;

	static void RegisterComponents(Context* context);
	static void RegisterEvents(Context* context);

	// game info text and timer to hide info
	WeakPtr<Text> gameInfoText_;
	Timer gameInfoTimer_;

	LGameSpaceState(Context * context);
	virtual ~LGameSpaceState();

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

	virtual void EnterSubstate(S32 stateNum);
	virtual void ExitSubstate();

	void CreateGame();
	void LoadGame();
	void EnterGame();

	void CreateSubstates();

	void SetupCamera();
	void SetupViewport() const;

	// Create logic
	void CreateBackground() const;
	void CreateGalaxy() const;
	void CreateUI();
	void CreateMap();

	void UpdateMapInfo();
	void HandleResearchUpdate(StringHash eventType, VariantMap& eventData);
	void HandleGameOver(StringHash eventType, VariantMap& eventData);

	void SubscribeToEvents();
	void HandlePhysicsPreStep(StringHash eventType, VariantMap& eventData);
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleIngameInfo(StringHash eventType, VariantMap& eventData);

	/// Handle game click
	void HandleMenuClick(StringHash eventType, VariantMap& eventData);
	void HandleKnowledgeClick(StringHash eventType, VariantMap& eventData);
	void HandleMapClick(StringHash eventType, VariantMap& eventData);
	void HandleShipInfoClick(StringHash eventType, VariantMap& eventData);
	void HandlePlanetClick(StringHash eventType, VariantMap& eventData);

	void HandleToPlanetClick(StringHash eventType, VariantMap& eventData);

	void StarSystemHyperDrive(StringHash eventType, VariantMap& eventData);

	void HandleZoomCamera(StringHash eventType, VariantMap& eventData);
	void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

	void HandleClientSceneLoaded(StringHash eventType, VariantMap & eventData);
	void HandlePlayerRemoved(StringHash eventType, VariantMap & eventData);

private:
	SharedPtr<LCollisionHandler> collisionHandler_;
	SharedPtr<LShipEquipment> shipEquipment_;
	SharedPtr<LSpaceBuildings> spaceBuildings_;
	SharedPtr<LKnowledgeUI> knowledgeUI_;
	SharedPtr<LStatusLineUI> statusLineUI_;
	SharedPtr<LFindObjectsHelper> findObjectsHelper_;

	WeakPtr<UIElement>	uiSpaceState_;
	Vector<ISubState*>	subStates_;
	SubStates			activeSubState_;

	/// Camera scene node.
	WeakPtr<Camera> camera_;

	// buttons
	WeakPtr<Button>              menuButton_;
	WeakPtr<Button>              mapButton_;
	WeakPtr<Button>              shipInfoButton_;
	WeakPtr<Button>              knowledgeButton_;
	WeakPtr<Button>              planetButton_;

	// map
	WeakPtr<UIElement>           mapWindow_;

	// object info window
	WeakPtr<UIElement>           objectInfoWindow_;

	F32 gRadius_;
	WeakPtr<Sprite> mySprite_;
	bool newStarSystem_ = false;

	struct StarSystemMapInfo
	{
		WeakPtr<Button> starSystemButton_;
		WeakPtr<Text> jumpCost_;
		WeakPtr<Sprite> sunSprite_;

		struct PlanetMapInfo
		{
			WeakPtr<Sprite> planetSprite_;
			WeakPtr<Text> raceText_;
		};
		HashMap<U32, PlanetMapInfo> planets_;
	};
	HashMap<U32, StarSystemMapInfo> starSystems_;

	struct WayMapInfo
	{
		Vector2 direction;
		F32 currentAlpha = 1.0f;
		Vector2 targetPos;
		WeakPtr<Sprite> sprite;
	};
	Vector<WayMapInfo> ways_;

	/// planet
	WeakPtr<Button>	toPlanetButton_;
	Node* planetNodeMove_;

	bool drawDebug_;
};
