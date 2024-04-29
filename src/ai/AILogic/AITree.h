#pragma once

#include <memory>

#include "utility/simpleTypes.h"

namespace Urho3D
{
	class XMLElement;
	class Node;
	class XMLFile;
	class Context;
}

namespace AI
{
class AINode;

/**
* Declaration of a Behavior tree
*/
class BehaviorTree final
{
public:
	BehaviorTree();
	BehaviorTree(Urho3D::Context* context, Urho3D::XMLFile* xmlFile);
	~BehaviorTree();

	void Process(F32 timeStep);
	std::unique_ptr<BehaviorTree> Clone() const;

	void LoadBehaviorFromXML(Urho3D::Context* context, Urho3D::XMLFile* xmlFile);
	void SetRunningDynamicNode(AINode* node);

	void SetAiElement(Urho3D::Node* aiElement);

private:
	static std::unique_ptr<AINode> CreateNode(Urho3D::Context* context, const Urho3D::XMLElement& xmlNode);

	std::unique_ptr<AINode> aiRoot_;
	AINode* runningDynamicNode_;
};
};
