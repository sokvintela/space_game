#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>

#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/managers/tDataManager.h"

#include "tacticalMode/starSystem/tDebris.h"

using namespace Urho3D;

void TDebris::RegisterObject(Context * context)
{
	context->RegisterFactory<TDebris>();
}

TDebris::TDebris(Context* context) :
	LogicComponent(context)
{
}

TDebris::~TDebris()
{
}

void TDebris::Create() const
{
	F32 value = Random(0.f, 400.f);

	TResourceManager* rManager = node_->CreateComponent<TResourceManager>();
	rManager->Create();
	rManager->AddResource(TGameResource::ResourceType::Metals, value);

	TDataManager* dManager = node_->CreateComponent<TDataManager>();
	dManager->SetObjectName("Debris");
	dManager->infoWindow_.Push(String(static_cast<U32>(value)) + " metals");

	node_->SetScale2D(Vector2(value, value) * 0.001f);
}
