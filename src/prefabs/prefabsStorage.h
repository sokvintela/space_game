#pragma once

#include <Urho3D/Core/Object.h>

#include <set>

#include "tacticalMode/equipment/tEquipment.h"
#include "legacyMode/buildings/lBuilding.h"
#include "legacyMode/equipment/lEquipment.h"

class PrefabsStorage : public Urho3D::Object
{
	URHO3D_OBJECT(PrefabsStorage, Urho3D::Object);

public:
	PrefabsStorage(Urho3D::Context* context);
	~PrefabsStorage() = default;

	void Load();

	// equipment
	const Urho3D::Node* GetEquipment(TEquipment::EquipmentType equipmentType, const String& equipmentName) const;
	Urho3D::Node* CreateEquipment(Node* playerNode, TEquipment::EquipmentType equipmentType, const String& equipmentName) const;
	bool IsEquipmentAvailable(Node* playerNode, TEquipment::EquipmentType equipmentType, const String& equipmentName) const;
	const HashMap<Urho3D::String, Urho3D::Node*> GetAvailabeEquipment(TEquipment::EquipmentType equipmentType, Urho3D::Node* playerNode) const;
	const HashMap<Urho3D::String, Urho3D::Node*> GetAllEquipment(TEquipment::EquipmentType equipmentType) const;

	// ships
	Urho3D::Node* GetMob() const;
	Urho3D::Node* GetBaseShip() const;

	// space objects
	Urho3D::Node* GetStar() const;
	Urho3D::Node* GetPlanet() const;

	Urho3D::Node* GetAsteroid() const;
	Urho3D::Node* GetDebris() const;

	// player logic
	Urho3D::Node* GetTeam() const;
	Urho3D::Node* GetPlayer() const;

	//costs
	Pair<TGameResource::ResourceType, F32> GetEquipmentCreateCost(TEquipment::EquipmentType equipmentType, const String& equipmentTypeName) const;
	Pair<TGameResource::ResourceType, F32> GetMobCreateCost() const;

	// LEGACY
	// equipment
	const Urho3D::Node* GetLegacyEquipment(LEquipment::EquipmentType equipmentType, const String& equipmentName) const;
	Urho3D::Node* CreateLegacyEquipment(Node* playerNode, LEquipment::EquipmentType equipmentType, const String& equipmentName) const;
	bool IsLegacyEquipmentAvailable(Node* playerNode, LEquipment::EquipmentType equipmentType, const String& equipmentName) const;
	const HashMap<Urho3D::String, Urho3D::Node*> GetAvailabeLegacyEquipment(LEquipment::EquipmentType equipmentType, Urho3D::Node* playerNode) const;

	// buildings
	const Urho3D::Node* GetBuilding(LBuilding::TargetType targetType, const String& buildingName) const;
	Urho3D::Node* CreateBuilding(Node * playerNode, LBuilding::TargetType targetType, const String& buildingName) const;
	bool IsBuildingAvailable(Node * playerNode, LBuilding::TargetType targetType, const String& buildingName) const;
	const HashSet<Urho3D::String> GetAvailableBuildings(Node* playerNode, LBuilding::TargetType targetType) const;

	// ships
	Urho3D::Node* CreateProbe(Node* playerNode) const;
	Urho3D::Node* GetLegacyBaseShip() const;

	// space objects
	Urho3D::Node* GetLegacyStar() const;
	Urho3D::Node* GetLegacyPlanet() const;

	Urho3D::Node* GetLegacyAsteroid() const;
	Urho3D::Node* GetLegacyDebris() const;

	// player logic
	Urho3D::Node* GetLegacyTeam() const;
	Urho3D::Node* GetLegacyPlayer() const;

	//costs
	Pair<LGameResource::ResourceType, F32> GetLegacyEquipmentCreateCost(LEquipment::EquipmentType equipmentType, const String& equipmentTypeName) const;
	Pair<LGameResource::ResourceType, F32> GetBuildingCreateCost(LBuilding::TargetType targetType, const String& buildingTypeName) const;
	Pair<LGameResource::ResourceType, F32> GetProbeCreateCost() const;

private:
	Urho3D::SharedPtr<Urho3D::Scene> scene_;

	// equipment
	Vector<HashMap<Urho3D::String, Urho3D::Node*>> equipment_;
	void LoadEquipmentFromXML(TEquipment::EquipmentType equipmentType, const String& xmlName);

	// ships
	Urho3D::Node* mobNode_;
	Urho3D::Node* baseShipNode_;

	// space objects
	Urho3D::Node* starNode_;
	Urho3D::Node* planetNode_;

	Urho3D::Node* asteroidNode_;
	Urho3D::Node* debrisNode_;

	// player logic
	Urho3D::Node* teamNode_;
	Urho3D::Node* playerNode_;

	// LEGACY
	Vector<HashMap<Urho3D::String, Urho3D::Node*>> legacyEquipment_;
	void LoadLegacyEquipmentFromXML(LEquipment::EquipmentType equipmentType, const String& xmlName);

	// buildings
	Vector<HashMap<Urho3D::String, Urho3D::Node*>> buildings_;
	void LoadBuildingFromXML(LBuilding::TargetType targetType, const String& xmlName);

	// ships
	Urho3D::Node* probeNode_;
	Urho3D::Node* legacyBaseShipNode_;

	// space objects
	Urho3D::Node* legacyStarNode_;
	Urho3D::Node* legacyPlanetNode_;

	Urho3D::Node* legacyAsteroidNode_;
	Urho3D::Node* legacyDebrisNode_;

	// player logic
	Urho3D::Node* legacyTeamNode_;
	Urho3D::Node* legacyPlayerNode_;
};
