#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include <set>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class LBuilding;

class LBuildingsManager : public LogicComponent
{
	URHO3D_OBJECT(LBuildingsManager, LogicComponent);

public:
	static StringHash buildingsStr;
	HashMap<U32, S8*> objectBuildingTypeNames;

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }

	LBuildingsManager(Context* context);
	~LBuildingsManager() = default;

	void Create() const;

	HashSet<String> GetProposalBuildings(Node* playerNode) const;
	PODVector<Node*> GetCompletedBuildings() const;
	HashSet<String> GetAvailableBuildings(Node* playerNode) const;

	Node* GetBuildingNode(String buildingName) const;
	LBuilding* GetBuilding(String buildingName) const;
	void AddBuilding(String buildingName, U32 ownerID) const;
	void ImproveBuilding(String buildingName);

	void AddOrChangeObjectOwner(S32 ownerID) const;
	void AddOrChangeObjectTeam(S32 teamNumber) const;
};
