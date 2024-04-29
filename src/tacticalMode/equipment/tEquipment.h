#pragma once

#include <Urho3D/Core/Timer.h>

#include "tacticalMode/resources/tGameResource.h"
#include "legacyMode/playerLogic/researchTypes.h"

using namespace Urho3D;

static const U32 THIDDEN_EQIUPMENT_VIEW_MASK = 0x00000001;

class TEquipment : public LogicComponent
{
	URHO3D_OBJECT(TEquipment, LogicComponent);

public:

	enum EquipmentType
	{
		ET_Engine,
		ET_FieldGenerator,
		ET_Weapon,
		ET_Count
	};

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }

	TEquipment(Context* context);
	~TEquipment();

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
	TGameResource::ResourceType useResource_;

	F32 timeUseMs_;
	Timer timerToUse_;
};
