#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Urho2D/RigidBody2D.h>

#include "utility/generalMethods.h"
#include "tacticalMode/starSystem/gravityAffected.h"
#include "tacticalMode/playerLogic/tTeam.h"

#include "tacticalMode/starSystem/gravity.h"

static const F32 constanta = 10000.f;

using namespace Urho3D;

void Gravity::RegisterObject(Context * context)
{
	context->RegisterFactory<Gravity>();
}

Gravity::Gravity(Context* context) :
	LogicComponent(context)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);
}

Gravity::~Gravity()
{
}

void Gravity::Update(F32 timeStep)
{
	Network* network = GetSubsystem<Network>();
	Connection* serverConnection = network->GetServerConnection();
	if (serverConnection)
		return;

	Vector<Node*> objectNodes;

	TStarSystem* system = GeneralMethods::GetOnlyComponent<TStarSystem>(node_);
	if (!system)
		return;

	HashSet<U32> teamIDs = system->teamIDs_;
	for (U32 teamID : teamIDs)
	{
		TTeam* team = GeneralMethods::GetComponent<TTeam>(node_, teamID);
		if (!team)
			continue;

		objectNodes.Push(GeneralMethods::GetObjectNode(node_, team->baseShipID_));

		HashSet<U32> playerIDs = team->playerIDs_;
		for (U32 playerID : playerIDs)
			objectNodes.Push(GeneralMethods::GetObjectNode(node_, playerID));

		HashSet<U32> mobIDs = team->mobIDs_;
		for (U32 mobID : mobIDs)
			objectNodes.Push(GeneralMethods::GetObjectNode(node_, mobID));
	}

	for (U32 i = 0; i < objectNodes.Size(); i++)
	{
		if (!objectNodes[i])
			continue;

		Vector2 direction = node_->GetWorldPosition2D() - objectNodes[i]->GetWorldPosition2D();
		F32 distance = direction.Length();
		direction.Normalize();

		RigidBody2D* rBody = objectNodes[i]->GetComponent<RigidBody2D>();
		if (rBody)
			rBody->ApplyForceToCenter(direction * ( constanta / ( distance * distance ) ), true);
	}
}
