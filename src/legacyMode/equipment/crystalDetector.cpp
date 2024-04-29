#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>

#include "legacyMode/galaxy/spaceObjects/crystal.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/galaxy/lStarSystem.h"
#include "legacyMode/managers/lDataManager.h"
#include "tacticalMode/equipment/tEquipment.h"

#include "legacyMode/equipment/crystalDetector.h"

using namespace Urho3D;

void CrystalDetector::RegisterObject(Context* context)
{
	context->RegisterFactory<CrystalDetector>();
}

CrystalDetector::CrystalDetector(Context* context) :
	LogicComponent(context)
{
}

bool CrystalDetector::Use() const
{
	TEquipment* equipment = node_->GetComponent<TEquipment>();
	if (equipment && equipment->Use())
		return true;

	LDataManager* dManager = node_->GetParentComponent<LDataManager>(true);
	if (!dManager)
		return false;

	bool isFastGame = false;

	LStarSystem* system = LGeneralMethods::GetStarSystem(node_, dManager->GetCurrentStarSystemID());
	Crystal* crystal = nullptr;
	if (system && !system->HasCrystal()
		&& system->IsCrystalReadyToCreate())
			crystal = system->CreateCrystal();
	/*
	else if (isFastGame
	&& tSystem && !tSystem->HasCrystal()
	&& tSystem->IsCrystalReadyToCreate())
	crystal = system->CreateCrystal();
	*/

	if (crystal)
	{
		crystal->SetVisible(true);
		return true;
	}

	return false;
}
