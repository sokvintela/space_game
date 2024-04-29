#include "ai/AILogic/AINode.h"

namespace AI
{
void AINode::AddChild(NodeUniquePtr childNode)
{
	childNodes_.push_back(std::move(childNode));
}

std::vector<AINode::NodeUniquePtr> AINode::CloneChildren() const
{
	std::vector<NodeUniquePtr> children;
	for (auto& child : childNodes_)
	{
		children.push_back(child->Clone());
	}

	return children;
}

bool AINode::initFromXML(const Urho3D::XMLElement& xmlNode)
{
	return true;
}

NodeReturnStatus AINode::Interrupt(F32 timeStep)
{
	for (auto& child : childNodes_)
	{
		NodeReturnStatus status = child->Interrupt(timeStep);
		if (status == NodeReturnStatus::Running)
			return status;
	}

	return NodeReturnStatus::Success;
}

void AINode::SetAiElement(Urho3D::Node* aiElement)
{
	aiElement_ = aiElement;
	for (auto& child : childNodes_)
	{
		child->SetAiElement(aiElement);
	}
}

void AINode::debugPrint() const
{
	for (auto& child : childNodes_)
	{
		child->debugPrint();
	}
}
};
