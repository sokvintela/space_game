#pragma once

#include "stateManager/gameStates.h"
#include "utility/simpleTypes.h"

namespace Urho3D
{
	class Window;
	class Sprite;
	class Button;
	class Text;
};

class LGameSpaceMapState : public ISubState
{
	URHO3D_OBJECT(LGameSpaceMapState, ISubState);

public:
	static void RegisterEvents(Context * context);

	LGameSpaceMapState(Urho3D::Context * context, UIElement* uiSubState);
	virtual ~LGameSpaceMapState() = default;

	virtual void Create(Node* playerNode = nullptr);
	virtual void Enter(Node* playerNode = nullptr);
	virtual void Exit();

private:
	F32 gRadius_;
	WeakPtr<Window> window_;
	WeakPtr<Node> playerNode_;
	WeakPtr<Sprite> mySprite_;
	WeakPtr<Button> backButton_;

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
		Vector2 direction_;
		F32 currentAlpha_ = 1.0f;
		Vector2 targetPos_;
		WeakPtr<Sprite> sprite_;
	};
	Vector<WayMapInfo> ways_;

	void StarSystemHyperDrive(StringHash eventType, VariantMap& eventData);
	void HandleBackClick(StringHash eventType, VariantMap& eventData);
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
};
