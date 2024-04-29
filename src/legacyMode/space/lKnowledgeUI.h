#pragma once

#include <Urho3D/Core/Object.h>
#include "utility/simpleTypes.h"

using namespace Urho3D;

namespace Urho3D
{
	class Node;
	class UIElement;
	class Button;
	class ListView;
};

class LKnowledgeUI : public Object
{
	URHO3D_OBJECT(LKnowledgeUI, Object);

public:
	LKnowledgeUI(Context* context, Node* controlNode, UIElement* uiSpaceState);
	~LKnowledgeUI();

	void HandleUpdateKnowledges();
	void SwitchKnowledgeUI() const;

private:
	Node* controlNode_;

	// middle UI
	WeakPtr<ListView> availableResearchList_;
	WeakPtr<ListView> completedResearchList_;

	HashMap<Button*, U32>   researchMapping_;

	void HandleSelectKnowledge(StringHash eventType, VariantMap& eventData);
};
