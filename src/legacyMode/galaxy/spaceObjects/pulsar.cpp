#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Network/Network.h>

#include "utility/simpleTypes.h"

#include "legacyMode/galaxy/spaceObjects/pulsar.h"

void Pulsar::RegisterObject(Context* context)
{
	context->RegisterFactory<Pulsar>();

	URHO3D_ATTRIBUTE("current_scale", F32, current_scale_, 1.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("step", F32, step_, 0.1f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("pulsarInitialScale", F32, pulsarInitialScale_, 0.0f, AM_DEFAULT);
}

Pulsar::Pulsar(Context * context) :
	LStar(context),
	current_scale_(1.f),
	step_(0.1f)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

void Pulsar::SetPulsarInitialScale(Vector2 scale)
{
	pulsarInitialScale_ = scale;
	MarkNetworkUpdate();
}

/*
void Pulsar::SetScale(float start_scale, float end_scale)
{
start_scale_ = start_scale;
end_scale_ = end_scale;
current_scale_ = start_scale + Random(end_scale - start_scale);
step_ = (end_scale - start_scale) / 100.0f;

MarkNetworkUpdate();
}
*/

void Pulsar::ChangeEnergySourceScale(F32 timeStep)
{
	F32 start_scale = 1.f;
	F32 end_scale = 1.5f;

	if (current_scale_ <= start_scale || current_scale_ >= end_scale)
		step_ = -step_;

	current_scale_ += step_ * timeStep;
	node_->SetScale2D(pulsarInitialScale_ * current_scale_);

	MarkNetworkUpdate();
}

void Pulsar::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	ChangeEnergySourceScale(timeStep);
}
