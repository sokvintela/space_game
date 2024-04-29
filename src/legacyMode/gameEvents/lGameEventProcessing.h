#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

class LGameEventProcessing : public Object
{
	URHO3D_OBJECT(LGameEventProcessing, Object);

public:
	LGameEventProcessing(Urho3D::Context * context);
	~LGameEventProcessing();

	virtual void SubscribeToNetworkEvents();

	void SetScene(Scene* scene);
	bool IsServerConnection() const;

	// OBJECT
	void HandleObjectBuild(StringHash eventType, VariantMap& eventData);
	void HandleTakeResource(StringHash eventType, VariantMap& eventData);
	void HandleImproveBuilding(StringHash eventType, VariantMap& eventData);

	// PLANET
	void HandleCreateProbe(StringHash eventType, VariantMap& eventData);
	void HandleCreateEquipment(StringHash eventType, VariantMap& eventData);
	void HandleStartMobsProduction(StringHash eventType, VariantMap& eventData);
	void HandleShipRepair(StringHash eventType, VariantMap& eventData);
	void HandleScrap(StringHash eventType, VariantMap& eventData);

	// MAP
	void HandleHyperDrive(StringHash eventType, VariantMap& eventData);

	// CRYSTAL
	void HandleCrystalDetectorUse(StringHash eventType, VariantMap& eventData);

	// Ship info
	void HandleEquipmentMove(StringHash eventType, VariantMap& eventData);

	// RESEARCH
	void HandleSelectResearch(StringHash eventType, VariantMap& eventData);

private:
	WeakPtr<Scene> scene_;
};
