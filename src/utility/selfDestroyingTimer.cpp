#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "utility/selfDestroyingTimer.h"

SelfDestroyingTimer::SelfDestroyingTimer(Context* context)
	: LogicComponent(context)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

void SelfDestroyingTimer::RegisterObject(Context * context)
{
	context->RegisterFactory<SelfDestroyingTimer>();

	URHO3D_ATTRIBUTE("lifeTime", F32, lifeTime_, 0.f, AM_DEFAULT);
}

void SelfDestroyingTimer::SetLifeTime(F32 lifeTime)
{
	lifeTime_ = lifeTime;
	timer_.Reset();

	MarkNetworkUpdate();
}

void SelfDestroyingTimer::Update(F32 timeStep)
{
	if (timer_.GetMSec(false) > lifeTime_)
		node_->Remove();
}
