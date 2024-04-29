#pragma once

#include <memory>
#include <vector>

#include "utility/simpleTypes.h"

namespace Urho3D
{
	class Node;
	class XMLElement;
}

namespace AI
{
enum class NodeReturnStatus
{
	Unknown = 0,

	Success,
	Failure,
	Running,

	Count
};

/**
* Declaration of a node interface
*/
class AINode
{
public:
	using NodeUniquePtr = std::unique_ptr<AINode>;

	AINode() = default;
	virtual ~AINode() = default;

	virtual bool initFromXML(const Urho3D::XMLElement& xmlNode);
	// Note: in clone functions we do not copy mAiPlayer, because the purpose of copying is to provide tree for another AI
	virtual NodeUniquePtr Clone() const = 0;
	virtual NodeReturnStatus Process(F32 timeStep) = 0;
	virtual NodeReturnStatus Interrupt(F32 timeStep);

	virtual void debugPrint() const;

	std::vector<NodeUniquePtr> CloneChildren() const;
	void AddChild(NodeUniquePtr childNode);
	void SetAiElement(Urho3D::Node* aiElement);

protected:

	std::vector<NodeUniquePtr> childNodes_;
	Urho3D::Node* aiElement_;
};
};
