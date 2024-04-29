#pragma once

#include <Urho3D/Core/Object.h>

using namespace Urho3D;

namespace Urho3D
{
	class Node;
	class UIElement;
	class Text;
};

class TStatusLineUI : public Object
{
	URHO3D_OBJECT(TStatusLineUI, Object);

public:
	TStatusLineUI(Context* context, Node* controlNode, UIElement* uiSpaceState);
	~TStatusLineUI();

	void UpdateShipInfo() const;
private:
	Node* controlNode_;

	// resources
	WeakPtr<Text>                choosenWeaponsText_;
	WeakPtr<Text>                shieldText_;
	Vector<WeakPtr<Text>>        resourceValueText_;
};
