#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>

#include "legacyMode/managers/lResourceManager.h"

#include "legacyMode/equipment/lEquipment.h"

using namespace Urho3D;

void LEquipment::RegisterObject(Context * context)
{
	context->RegisterFactory<LEquipment>();

	URHO3D_ATTRIBUTE("freeUse", bool, freeUse_, false, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("technology", technology_, researchTypeNames, Research::ResearchType::Count, AM_DEFAULT);
	URHO3D_ENUM_ATTRIBUTE("useResource", useResource_, lResourceTypeNames, LGameResource::ResourceType::Count, AM_DEFAULT);
	URHO3D_ATTRIBUTE("timeUseMs", F32, timeUseMs_, 0.0f, AM_DEFAULT);
	URHO3D_ATTRIBUTE("useCost", F32, useCost_, 0.0f, AM_DEFAULT);
}

LEquipment::LEquipment(Context* context) :
	LogicComponent(context),
	freeUse_(false),
	useCost_(0.f),
	useResource_(LGameResource::ResourceType::Energy),
	technology_(Research::ResearchType::Base)
{
	timeUseMs_ = 0.0f;
}

LEquipment::~LEquipment()
{
}

F32 LEquipment::GetUseCost() const
{
	return useCost_;
}

void LEquipment::SetUseCost(F32 useCost)
{
	useCost_ = useCost;
	MarkNetworkUpdate();
}

void LEquipment::SetFreeToUse()
{
	freeUse_ = true;
	MarkNetworkUpdate();
}

bool LEquipment::IsReadyToUse()
{
	if (timerToUse_.GetMSec(false) < timeUseMs_)
		return false;

	return true;
}

bool LEquipment::Use()
{
	if (!IsReadyToUse())
		return false;

	if (!freeUse_)
	{
		LResourceManager* resourceManager = node_->GetParentComponent<LResourceManager>(true);
		if (!resourceManager || !resourceManager->ChangeResourceValue(useResource_, -useCost_))
			return false;
	}

	ResetTimer();
	return true;
}

void LEquipment::ResetTimer()
{
	timerToUse_.Reset();
	MarkNetworkUpdate();
}
