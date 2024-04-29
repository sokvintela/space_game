#pragma once

#include <Urho3D/Core/Object.h>

using namespace Urho3D;

namespace Urho3D
{
	class Node;
	class UIElement;
};

class LSpaceBuildings : public Object
{
	URHO3D_OBJECT(LSpaceBuildings, Object);

public:
	LSpaceBuildings(Context* context, Node* controlNode, UIElement* uiSpaceState);
	~LSpaceBuildings();

	void UpdateBuildingsUI(Node* enteredNode);

	void UpdateProposalBuildingsUI(Node* enteredNode);
	void UpdateCompletedBuildingsUI(Node* enteredNode);

	void SwitchOff();

private:
	void HandleObjectBuild(StringHash eventType, VariantMap& eventData);
	void HandlePlanetOpen(StringHash eventType, VariantMap& eventData);

	Node* controlNode_;
	Node* enteredNode_;

	// buildings
	WeakPtr<UIElement>           buildingWindow_;

	WeakPtr<UIElement>           proposalBuildingsWindow_;
	WeakPtr<UIElement>           completedBuildingsWindow_;
};
