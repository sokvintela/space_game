#pragma once

#include <Urho3D/Core/Object.h>

#include "tacticalMode/managers/tEquipmentManager.h"

using namespace Urho3D;

namespace Urho3D
{
	class Node;
	class UIElement;
	class Sprite;
	class Button;
};

class TShipEquipment : public Object
{
	URHO3D_OBJECT(TShipEquipment, Object);

public:
	TShipEquipment(Context* context, Node* controlNode, UIElement* uiSpaceState);
	~TShipEquipment();

	void CreateShipInfo();

	// ItemsHandling
	void HandleDragBegin(StringHash eventType, VariantMap& eventData);
	void HandleDragMove(StringHash eventType, VariantMap& eventData);
	void HandleDragEnd(StringHash eventType, VariantMap& eventData);
	void HandleNodeAdded(StringHash eventType, VariantMap& eventData);
	void HandleShipInfoUpdate();
	// ---

private:
	bool IsPlacementCorrect(TEquipmentManager::Module moduleType, Node* node) const;

	Node* controlNode_;

	/// ship info
	WeakPtr<UIElement> equipmentBar_;
	WeakPtr<UIElement> modulesBar_;
	WeakPtr<UIElement> cargoBar_;

	IntVector2              dragBeginElementPosition_;
	IntVector2              dragBeginPosition_;

	WeakPtr<Sprite>         leftWeaponPlace_;
	WeakPtr<Sprite>         enginePlace_;
	WeakPtr<Sprite>         rightWeaponPlace_;
	WeakPtr<Button>         leftWeaponButton_;
	WeakPtr<Button>         rightWeaponButton_;
	WeakPtr<Button>         engineButton_;
};
