#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include <memory>

#include "utility/simpleTypes.h"

namespace AI
{
	class BehaviorTree;
};

using namespace Urho3D;

class AIBase : public LogicComponent
{
	URHO3D_OBJECT(AIBase, LogicComponent);

public:

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }

	AIBase(Context* context);
	~AIBase() = default;

	void Activate(bool status);
	const String& GetBehavior() const;
	void SetBehavior(const String& name);
	void RemoveBehavior();

	AI::BehaviorTree* GetBehaviorTree() const;

private:
	std::unique_ptr<AI::BehaviorTree> behaviorTree_;
	String behaviorFileName_;
	bool isActive_;

	void Update(F32 timeStep);
};
