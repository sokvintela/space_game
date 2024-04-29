#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Timer.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class LResearchTree : public LogicComponent
{
	URHO3D_OBJECT(LResearchTree, LogicComponent);

public:

	struct ResearchItem
	{
		String name_;
		F32 baseCost_;
		bool isInProcess_;
		bool isCompleted_;

		HashSet<U32> prerequisiteItems_;

		ResearchItem() = default;
		ResearchItem(const String& name, F32 baseCost);
	};

	static void RegisterObject(Context* context);
	LResearchTree(Context* context);
	~LResearchTree();

	void SetResearchedItemsAttr(const PODVector<U8>& value);
	PODVector<U8> GetResearchedItemsAttr() const;

	bool PrerequisitesReady(U32 researchItemType);
	HashSet<U32> GetAvailableResearches();
	HashSet<U32> GetCompletedResearches() const;

	void StartResearch(U32 researchItemType);

	F32 GetResearchCost(U32 researchItemType);
	String GetResearchName(U32 researchItemType);

private:
	HashMap<U32, ResearchItem*> allResearchItems_;

	Timer researchTimer_;

	virtual void Update(F32 timeStep);
};
