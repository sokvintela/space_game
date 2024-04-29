#pragma once

#include <memory>
#include <unordered_map>

#include "utility/stringHasher.h"

using namespace Urho3D;

namespace AI
{
class AINodeStorage;
class BehaviorTree;
class AINode;

class AIManager : public Object
{
	// Enable type information.
	URHO3D_OBJECT(AIManager, Object);

public:

	/// Construct.
	AIManager(Context* context);
	std::unique_ptr<AINode> GetNodeClone(const Urho3D::String& nodeClassName) const;
	std::unique_ptr<BehaviorTree> GetBehavior(const Urho3D::String& behaviorFileName) const;
	void LoadBehaviors();

private:
	void CreateAiNodesPrototypes();

	std::unique_ptr<AINodeStorage> aiNodeStorage_;
	std::unordered_map<Urho3D::String, std::unique_ptr<BehaviorTree>, StringHasher> behaviorAiMap_;
};
};
