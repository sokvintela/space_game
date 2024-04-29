#pragma once

#include <memory>
#include <unordered_map>

#include "utility/stringHasher.h"

namespace AI
{
class AINode;

class AINodeStorage
{
public:
	AINodeStorage();

	void RegisterNodeType(const Urho3D::String& nodeClassName, std::unique_ptr<AINode> node);
	std::unique_ptr<AINode> GetNodeClone(const Urho3D::String& nodeClassName) const;

private:
	std::unordered_map<Urho3D::String, std::unique_ptr<AINode>, StringHasher> aiNodeMap;
};
};
