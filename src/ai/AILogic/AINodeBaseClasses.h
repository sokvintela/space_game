#pragma once
#include <Urho3D/Math/Vector2.h>

#include "ai/AILogic/AINode.h"

namespace AI
{
namespace Nodes
{
/// Control flow nodes
class Selector : public AINode
{
public:
	Selector() = default;
	virtual ~Selector() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class Sequence : public AINode
{
public:
	Sequence() = default;
	virtual ~Sequence() = default;

	AINode::NodeUniquePtr Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class RndSelector : public AINode
{
public:
	RndSelector();
	RndSelector(Urho3D::Vector<F32> probabilities);
	virtual ~RndSelector() = default;

	AINode::NodeUniquePtr Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
private:

	Urho3D::Vector<F32> mProbabilities;
	U32 mRunningIndex;
};

class DynSelector : public AINode
{
public:
	DynSelector() = default;
	virtual ~DynSelector() = default;

	AINode::NodeUniquePtr Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class Interruptor : public AINode
{
public:
	Interruptor() = default;
	virtual ~Interruptor() = default;

	AINode::NodeUniquePtr Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
	NodeReturnStatus Interrupt(F32 timeStep) override;
};

class Inverter : public AINode
{
public:
	Inverter() = default;
	virtual ~Inverter() = default;

	AINode::NodeUniquePtr Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class Successor : public AINode
{
public:
	Successor() = default;
	virtual ~Successor() = default;

	AINode::NodeUniquePtr Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class Negator : public AINode
{
public:
	Negator() = default;
	virtual ~Negator() = default;

	AINode::NodeUniquePtr Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};
};
};
