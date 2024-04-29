#pragma once

#include "stateManager/gameStates.h"
#include "legacyMode/equipment/lEquipment.h"

namespace Urho3D
{
	class Button;
	class Text;
	class Sprite;
	class ListView;
	class UIElement;
}

class LBuildingsManager;
class LEquipmentManager;

using namespace Urho3D;

class LGameSpacePlanetState : public ISubState
{
	URHO3D_OBJECT(LGameSpacePlanetState, ISubState);
public:

	static void RegisterEvents(Context * context);

	LGameSpacePlanetState(Urho3D::Context * context, UIElement * uiSubState);
	virtual ~LGameSpacePlanetState() = default;

	virtual void Create(Node* playerNode = nullptr);
	virtual void Enter(Node* planetNode);
	virtual void Exit();
private:

	void ShowEquipment(LEquipment::EquipmentType equipmentType);
	void UpdateProposalBuildings();
	void UpdateCompleteBuildings();

	WeakPtr<LBuildingsManager> buildingManager_;
	WeakPtr<LEquipmentManager> equipmentManager_;

	// spaceship nodes
	WeakPtr<Node>           controlNode_;

	// planet
	WeakPtr<Node>           planetNode_;
	WeakPtr<Sprite>         planetImage_;

	WeakPtr<Text>           colonizedText_;
	WeakPtr<Text>           planetTypeText_;
	WeakPtr<Text>           oreWealthText_;
	WeakPtr<Text>           energyReceptionCapacityText_;


	// buildings for choise
	WeakPtr<UIElement>      buildChoicesBar_;
	WeakPtr<ListView>       buildChoicesList_;

	// all buildings
	WeakPtr<UIElement>      constructedBar_;
	WeakPtr<ListView>       completedBuildingsList_;

	// building info
	WeakPtr<UIElement>      buildingInfoBar_;
	WeakPtr<ListView>       buildingItemsList_;
	WeakPtr<Button>         improveBuildingButton_;
	WeakPtr<Button>         getResourceButton_;
	WeakPtr<Button>         closeBuildingButton_;

	// cargo
	WeakPtr<UIElement>      cargoElement_;
	WeakPtr<ListView>       cargoList_;
	bool                    isCargo_;

	// complete buildings


	// update buildings

	// probes
	WeakPtr<Text>			energyValue_;

	// ship factory
	Vector<WeakPtr<Button>>	createShipButton_;

	// orbital factory
	Vector<WeakPtr<Button>> createOrbitalBuildingButton_;

	// repair
	WeakPtr<Button> repairEnergyButton_;
	WeakPtr<Text>	repairEnergy_;
	WeakPtr<Button>	repairButton_;

	// scrap-metal
	Vector<WeakPtr<Button>> scrapButton_;
	HashMap<Button*, Node*> cargoMapping_;

	WeakPtr<Button>         backToSpace_;
	WeakPtr<Button>         showCargo_;

	// buildings
	void HandleBuildItemSelected(StringHash eventType, VariantMap& eventData);
	void HandleOpenBuildingInfo(StringHash eventType, VariantMap & eventData);

	void HandleTakeBuildingResourceDialog(StringHash eventType, VariantMap& eventData);
	void HandleImproveBuilding(StringHash eventType, VariantMap& eventData);

	void HandleCloseBuilding(StringHash eventType, VariantMap& eventData);

	// probe
	void HandleCreateProbe(StringHash eventType, VariantMap& eventData);

	// equipmentFactory
	void HandleCreateEquipment(StringHash eventType, VariantMap& eventData);

	// shipFactory
	void HandleCreateShipClick(StringHash eventType, VariantMap& eventData);

	// orbital factory
	void HandleCreateOrbitalBuilding(StringHash eventType, VariantMap& eventData);

	// repair
	void HandleRepair(StringHash eventType, VariantMap& eventData);

	// scrap-metal factory
	void HandleScrap(StringHash eventType, VariantMap& eventData);

	// to game
	void HandleBackToGameClick(StringHash eventType, VariantMap& eventData);
	void HandleShowCargoClick(StringHash eventType, VariantMap& eventData);

	void HandleUpdate(StringHash eventType, VariantMap& eventData);
};
