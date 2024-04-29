#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Scene/Node.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class TFindObjectsHelper : public Object
{
	URHO3D_OBJECT(TFindObjectsHelper, Object);

public:
	TFindObjectsHelper(Context* context, Node* controlNode, UIElement* uiObjectWindow);
	~TFindObjectsHelper();

	void FindClosestObjects(Vector2 mousePos, bool& closeToBase);

	Node* GetEnteredNode() const;
	Node* GetHoveringNode() const;

	void SetEnteredNode(Node* enteredNode);

private:
	Node* controlNode_;

	WeakPtr<Node> enteredNode_;
	WeakPtr<Node> hoveringNode_;

	WeakPtr<UIElement> objectWindow_;
	WeakPtr<Text> ownerText_;
	WeakPtr<Text> typeText_;
	WeakPtr<Text> durabilityText_;
	WeakPtr<Text> efficiencyText_;
	WeakPtr<Text> resourcesText_;
};
