#pragma once

#include "ai/AILogic/AINode.h"

namespace AI
{
namespace Nodes
{
class Fire : public AINode
{
public:
	Fire() = default;
	virtual ~Fire() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class FlyToEnemyBase : public AINode
{
public:
	FlyToEnemyBase() = default;
	virtual ~FlyToEnemyBase() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class Stop : public AINode
{
public:
	Stop() = default;
	virtual ~Stop() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class IsEnemyAround : public AINode
{
public:
	IsEnemyAround() = default;
	virtual ~IsEnemyAround() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};
};
};
