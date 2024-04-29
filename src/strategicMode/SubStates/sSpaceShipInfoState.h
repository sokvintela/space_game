#pragma once

#include "stateManager/gameStates.h"

namespace Urho3D
{
	class Button;
	class Sprite;
	class UIElement;
}

class TEquipmentManager;

using namespace Urho3D;

class SGameShipInfoState : public ISubState
{
	URHO3D_OBJECT(SGameShipInfoState, ISubState);
public:

	enum SlotType
	{
		ST_LEFT_WEAPON,
		ST_RIGHT_WEAPON,
		ST_ENGINE,
		ST_HYPER_DRIVE,
		ST_CARGO
	};

	static void RegisterEvents(Context * context);
	SGameShipInfoState(Urho3D::Context * context, UIElement * uiSubState);
	virtual ~SGameShipInfoState() = default;

	virtual void Create(Node* targetNode = nullptr);
	virtual void Enter(Node* playerNode);
	virtual void Exit();

private:
	int playerID_;
	WeakPtr<TEquipmentManager> equipmentManager_;

	// create
	// cargo
	WeakPtr<UIElement>		cargoBar_;

	// ship cells
	// left weapon
	WeakPtr<Sprite>         leftWeaponPlace_;
	WeakPtr<Button>         leftWeaponButton_;

	// right weapon
	WeakPtr<Sprite>         rightWeaponPlace_;
	WeakPtr<Button>         rightWeaponButton_;

	// left engine
	WeakPtr<Sprite>         enginePlace_;
	WeakPtr<Button>         engineButton_;

	// right engine
	WeakPtr<Sprite>         hyperEnginePlace_;
	WeakPtr<Button>         hyperEngineButton_;

	WeakPtr<Button>			backToSpace_;

	// enter state
	// ship
	WeakPtr<Node>           leftWing_;
	WeakPtr<Node>           rightWing_;

	HashMap<Sprite*, SlotType> slotTypeMapping_;
	HashMap<Button*, Node*> equipmentMapping_;
	// use
	// cargo
	IntVector2				dragBeginElementPosition_;
	IntVector2				dragBeginPosition_;

	void HandleDragBegin(StringHash eventType, VariantMap& eventData);
	void HandleDragMove(StringHash eventType, VariantMap& eventData);
	void HandleDragEnd(StringHash eventType, VariantMap& eventData);

	void HandleBackToGameClick(StringHash eventType, VariantMap& eventData);

	void HandleNodeAdded(StringHash eventType, VariantMap& eventData);
	void HandleShipInfoUpdate();
};
