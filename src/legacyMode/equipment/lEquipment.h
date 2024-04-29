#pragma once

#include <Urho3D/Core/Timer.h>

#include "legacyMode/resources/lGameResource.h"
#include "legacyMode/playerLogic/researchTypes.h"

using namespace Urho3D;

static const U32 LHIDDEN_EQIUPMENT_VIEW_MASK = 0x00000001;

class LEquipment : public LogicComponent
{
	URHO3D_OBJECT(LEquipment, LogicComponent);

public:

	enum EquipmentType
	{
		ET_Engine,
		ET_HyperDrive,
		ET_Weapon,
		ET_FieldGenerator,
		ET_CrystalDetector,
		ET_RepairDrone,
		ET_Count
	};

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }

	LEquipment(Context* context);
	~LEquipment();

	F32 GetUseCost() const;
	void SetUseCost(F32 useCost);

	void SetFreeToUse();

	bool IsReadyToUse();
	bool Use();
	void ResetTimer();

	Research::ResearchType technology_;

private:
	bool freeUse_;
	F32 useCost_;
	LGameResource::ResourceType useResource_;

	F32 timeUseMs_;
	Timer timerToUse_;
};
