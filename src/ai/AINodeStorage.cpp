#include "ai/AINodeStorage.h"

#include "ai/AILogic/AINode.h"

namespace AI
{
AINodeStorage::AINodeStorage()
{
}

void AINodeStorage::RegisterNodeType(const Urho3D::String& nodeClassName, std::unique_ptr<AINode> node)
{
	aiNodeMap[nodeClassName] = std::move(node);
}

std::unique_ptr<AINode> AINodeStorage::GetNodeClone(const Urho3D::String& nodeClassName) const
{
	auto nodeIt = aiNodeMap.find(nodeClassName);
	if (nodeIt == aiNodeMap.end() || nodeIt->second == nullptr)
	{
		return nullptr;
	}

	return nodeIt->second->Clone();
}
};
