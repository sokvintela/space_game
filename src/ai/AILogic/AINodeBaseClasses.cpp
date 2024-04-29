#include <Urho3D/Scene/Node.h>

#include "ai/AILogic/AITree.h"
#include "ai/AIBase.h"

#include "ai/AILogic/AINodeBaseClasses.h"

using namespace Urho3D;

namespace AI
{
namespace Nodes
{
std::unique_ptr<AINode> Selector::Clone() const
{
	auto newNode = std::make_unique<Selector>();
	newNode->childNodes_ = this->CloneChildren();
	return std::move(newNode);
}

NodeReturnStatus Selector::Process(F32 timeStep)
{
	for (auto& child : childNodes_)
	{
		NodeReturnStatus childStatus = child->Process(timeStep);
		if (childStatus == NodeReturnStatus::Success || childStatus == NodeReturnStatus::Running)
		{
			return childStatus;
		}
	}
	return NodeReturnStatus::Failure;
}

AINode::NodeUniquePtr Sequence::Clone() const
{
	auto newNode = std::make_unique<Sequence>();
	newNode->childNodes_ = this->CloneChildren();
	return std::move(newNode);
}

NodeReturnStatus Sequence::Process(F32 timeStep)
{
	for (auto& child : childNodes_)
	{
		NodeReturnStatus childStatus = child->Process(timeStep);
		if (childStatus == NodeReturnStatus::Failure || childStatus == NodeReturnStatus::Running)
		{
			return childStatus;
		}
	}
	return NodeReturnStatus::Success;
}

RndSelector::RndSelector()
	: mRunningIndex(-1)
{
}

RndSelector::RndSelector(Vector<F32> probabilities)
	: mProbabilities(probabilities), mRunningIndex(-1)
{
}

AINode::NodeUniquePtr RndSelector::Clone() const
{
	auto newNode = std::make_unique<RndSelector>(mProbabilities);
	newNode->childNodes_ = this->CloneChildren();
	return std::move(newNode);
}

NodeReturnStatus RndSelector::Process(float timeStep)
{
	if (childNodes_.empty())
	{
		return NodeReturnStatus::Success;
	}

	unsigned int chosenIndex = 0;
	if (mRunningIndex != -1)
	{
		chosenIndex = mRunningIndex;
	}
	else
	{
		while (mProbabilities.Size() < childNodes_.size())
		{
			mProbabilities.Push(0.0f);
		}

		float probabilityBase = 0.0f;
		for (U32 i = 0; i < mProbabilities.Size(); i++)
		{
			probabilityBase += mProbabilities[i];
		}

		float randomGenerated = Random();

		float probSumm = 0.0f;
		for (U32 i = 0; i < mProbabilities.Size(); i++)
		{
			probSumm += mProbabilities[i] / probabilityBase;
			if (probSumm > randomGenerated)
			{
				chosenIndex = i;
				break;
			}
		}
	}

	auto result = childNodes_[chosenIndex]->Process(timeStep);
	if (result == NodeReturnStatus::Running)
	{
		mRunningIndex = chosenIndex;
	}
	else
	{
		mRunningIndex = -1;
	}

	return result;
}

AINode::NodeUniquePtr DynSelector::Clone() const
{
	auto newNode = std::make_unique<DynSelector>();
	newNode->childNodes_ = this->CloneChildren();
	return std::move(newNode);
}

NodeReturnStatus DynSelector::Process(F32 timeStep)
{
	if (childNodes_.empty())
	{
		return NodeReturnStatus::Success;
	}

	for (U32 i = 0; i < childNodes_.size() - 1; i++)
	{
		NodeReturnStatus result = childNodes_[i]->Process(timeStep);
		if (result == NodeReturnStatus::Success)
		{
			childNodes_.back()->Interrupt(timeStep);
			return NodeReturnStatus::Failure;
		}
	}

	NodeReturnStatus result = childNodes_.back()->Process(timeStep);
	if (result == NodeReturnStatus::Running)
	{
		AIBase* aiBase = aiElement_->GetComponent<AIBase>();
		aiBase->GetBehaviorTree()->SetRunningDynamicNode(this);
	}

	return result;
}

AINode::NodeUniquePtr Interruptor::Clone() const
{
	auto newNode = std::make_unique<Interruptor>();
	newNode->childNodes_ = this->CloneChildren();
	return std::move(newNode);
}

NodeReturnStatus Interruptor::Process(F32 timeStep)
{
	return NodeReturnStatus::Success;
}

NodeReturnStatus Interruptor::Interrupt(F32 timeStep)
{
	NodeReturnStatus result0 = childNodes_[0]->Interrupt(timeStep);
	if (childNodes_.size() >= 2)
	{
		return childNodes_[1]->Process(timeStep);
	}
	else
	{
		return result0;
	}
}

AINode::NodeUniquePtr Inverter::Clone() const
{
	auto newNode = std::make_unique<Inverter>();
	newNode->childNodes_ = this->CloneChildren();
	return std::move(newNode);
}

NodeReturnStatus Inverter::Process(F32 timeStep)
{
	auto result = childNodes_[0]->Process(timeStep);
	switch (result)
	{
	case NodeReturnStatus::Success:
		return NodeReturnStatus::Failure;
	case NodeReturnStatus::Failure:
		return NodeReturnStatus::Success;
	case NodeReturnStatus::Running:
		return NodeReturnStatus::Running;
	default:
		return NodeReturnStatus::Unknown;
	}
}

AINode::NodeUniquePtr Successor::Clone() const
{
	auto newNode = std::make_unique<Successor>();
	newNode->childNodes_ = this->CloneChildren();
	return std::move(newNode);
}

NodeReturnStatus Successor::Process(F32 timeStep)
{
	if (!childNodes_.empty())
	{
		childNodes_[0]->Process(timeStep);
	}

	return NodeReturnStatus::Success;
}

AINode::NodeUniquePtr Negator::Clone() const
{
	auto newNode = std::make_unique<Negator>();
	newNode->childNodes_ = this->CloneChildren();
	return std::move(newNode);
}

NodeReturnStatus Negator::Process(F32 timeStep)
{
	if (!childNodes_.empty())
	{
		childNodes_[0]->Process(timeStep);
	}

	return NodeReturnStatus::Failure;
}
};
};
