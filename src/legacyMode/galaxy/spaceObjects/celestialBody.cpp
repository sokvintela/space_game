#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

#include "legacyMode/galaxy/spaceObjects/celestialBody.h"

void CelestialBody::RegisterObject(Context * context)
{
	context->RegisterFactory<CelestialBody>();

	URHO3D_ATTRIBUTE("orbit_radius", F32, orbitRadius_, 1.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("angle", F32, angle_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("angleStep", F32, angleStep_, M_PI / 250.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("angularSpeed", F32, angularSpeed_, 250.0f, AM_DEFAULT);
}

CelestialBody::CelestialBody(Context * context) :
	LogicComponent(context),
	orbitRadius_(1.0f),
	angularSpeed_(250.0f),
	angle_(0.0f)
{
	angleStep_ = M_PI / angularSpeed_;

	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

void CelestialBody::SetAngle(F32 angle)
{
	angle_ = angle;
	MarkNetworkUpdate();
}

void CelestialBody::SetRadius(F32 radius)
{
	orbitRadius_ = radius;
	MarkNetworkUpdate();
}

void CelestialBody::SetAngularSpped(F32 angularSpeed)
{
	angularSpeed_ = angularSpeed;
	angleStep_ = M_PI / angularSpeed_;

	MarkNetworkUpdate();
}

void CelestialBody::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	angle_ += angleStep_ * timeStep;
	if (angle_ >= 2 * M_PI)
		angle_ -= 2 * M_PI;

	MarkNetworkUpdate();

	node_->SetPosition2D(orbitRadius_ * cosf(angle_), orbitRadius_ * sinf(angle_));
}
