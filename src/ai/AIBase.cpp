#include <Urho3D/Core/Context.h>

#include "ai/AILogic/AITree.h"
#include "ai/AIManager.h"

#include "ai/AIBase.h"

using namespace Urho3D;

void AIBase::RegisterObject(Context* context)
{
	context->RegisterFactory<AIBase>();

	URHO3D_ATTRIBUTE("isActive", bool, isActive_, true, AM_DEFAULT);
	URHO3D_ACCESSOR_ATTRIBUTE("behaviour", GetBehavior, SetBehavior, String, String::EMPTY, AM_DEFAULT);
}

AIBase::AIBase(Context* context) :
	LogicComponent(context),
	isActive_(true)
{
	SetUpdateEventMask(USE_UPDATE);
}

void AIBase::Activate(bool status)
{
	isActive_ = status;
}

const String& AIBase::GetBehavior() const
{
	return behaviorFileName_;
}

void AIBase::SetBehavior(const String& name)
{
	AI::AIManager* aiManager = GetSubsystem<AI::AIManager>();
	if (!aiManager)
		return;

	behaviorFileName_ = name;
	behaviorTree_ = aiManager->GetBehavior(behaviorFileName_);
	if (behaviorTree_)
		behaviorTree_->SetAiElement(node_);
}

void AIBase::RemoveBehavior()
{
	behaviorTree_.reset();
}

AI::BehaviorTree* AIBase::GetBehaviorTree() const
{
	return behaviorTree_.get();
}

void AIBase::Update(F32 timeStep)
{
	if (isActive_ && behaviorTree_)
		behaviorTree_->Process(timeStep);
}
