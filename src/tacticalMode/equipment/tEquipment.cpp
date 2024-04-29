#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>

#include "tacticalMode/managers/tResourceManager.h"

#include "tacticalMode/equipment/tEquipment.h"

using namespace Urho3D;

void TEquipment::RegisterObject(Context * context)
{
	context->RegisterFactory<TEquipment>();

	URHO3D_ATTRIBUTE("freeUse", bool, freeUse_, false, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("technology", technology_, researchTypeNames, Research::ResearchType::Count, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("useResource", useResource_, tResourceTypeNames, TGameResource::ResourceType::Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("timeUseMs", F32, timeUseMs_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("useCost", F32, useCost_, 0.0f, AM_DEFAULT);
}

TEquipment::TEquipment(Context* context) :
	LogicComponent(context),
	freeUse_(false),
	useCost_(0.f),
	useResource_(TGameResource::ResourceType::Energy),
	technology_(Research::ResearchType::Base)
{
	timeUseMs_ = 0.0f;
}

TEquipment::~TEquipment()
{
}

F32 TEquipment::GetUseCost() const
{
	return useCost_;
}

void TEquipment::SetUseCost(F32 useCost)
{
	useCost_ = useCost;
	MarkNetworkUpdate();
}

void TEquipment::SetFreeToUse()
{
	freeUse_ = true;
	MarkNetworkUpdate();
}

bool TEquipment::IsReadyToUse()
{
	if (timerToUse_.GetMSec(false) < timeUseMs_)
		return false;

	return true;
}

bool TEquipment::Use()
{
	if (!IsReadyToUse())
		return false;

	if (!freeUse_)
	{
		TResourceManager* resourceManager = node_->GetParentComponent<TResourceManager>(true);
		if (!resourceManager || !resourceManager->ChangeResourceValue(useResource_, -useCost_))
			return false;
	}

	ResetTimer();
	return true;
}

void TEquipment::ResetTimer()
{
	timerToUse_.Reset();
	MarkNetworkUpdate();
}
