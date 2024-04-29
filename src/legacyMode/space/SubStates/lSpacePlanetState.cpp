#include <Urho3D/UI/Button.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Core/Context.h>

#include "tacticalMode/equipment/weapon/weapon.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/buildings/shipFactory.h"
#include "legacyMode/equipment/hyperDrive.h"
#include "equipment/shipEngine.h"
#include "tacticalMode/ai/mob.h"
#include "equipment/energyFieldGenerator.h"
#include "legacyMode/managers/lBuildingsManager.h"
#include "prefabs/prefabsStorage.h"
#include "legacyMode/managers/lEquipmentManager.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "legacyMode/equipment/repairDrone.h"
#include "legacyMode/equipment/crystalDetector.h"
#include "legacyMode/utility/lSharedData.h"
#include "legacyMode/spaceship/lDurability.h"
#include "legacyMode/managers/lDataManager.h"
#include "legacyMode/galaxy/spaceObjects/lPlanet.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/buildings/lBuilding.h"

#include "legacyMode/space/SubStates/lSpacePlanetState.h"

String buildingName = "buildingName";
String resource = "resource";
String equipmentType = "equipmentType";
String equipmentName = "equipmentName";
String targetType = "targetType";

using namespace Urho3D;

void LGameSpacePlanetState::RegisterEvents(Context * context)
{
	Network* network = context->GetSubsystem<Network>();
	network->RegisterRemoteEvent(RE_OBJECT_BUILD);
	network->RegisterRemoteEvent(RE_GET_BUILDING_RESOURCE);
	network->RegisterRemoteEvent(RE_IMPROVE_BUILDING);
	network->RegisterRemoteEvent(RE_CREATE_PROBE);
	network->RegisterRemoteEvent(RE_CREATE_EQUIPMENT);
	network->RegisterRemoteEvent(RE_MOBS_PRODUCTION);
	network->RegisterRemoteEvent(RE_SHIP_REPAIR);
	network->RegisterRemoteEvent(RE_PUT_ON_SCRAP);
}

LGameSpacePlanetState::LGameSpacePlanetState(Urho3D::Context * context, UIElement* uiSubState) :
	ISubState(context, uiSubState),
	isCargo_(false)
{
}

void LGameSpacePlanetState::Enter(Node* planetNode)
{
	// planet info gathering
	planetNode_ = planetNode;
	if (!planetNode_)
		return;

	LPlanet* planet = planetNode_->GetComponent<LPlanet>();
	if (planet && planet->GetPlanetType() == LPlanet::PlanetType::PT_Solid)
	{
		colonizedText_->SetText("Colonized");

		LDataManager* dManager = planetNode_->GetComponent<LDataManager>();
		if (dManager)
			planetTypeText_->SetText("Planet type: " + dManager->GetObjectName());

		oreWealthText_->SetText("Ore wealth: " + String(static_cast<unsigned>(dManager->GetObjectEfficiency())));
		energyReceptionCapacityText_->SetText("Energy reception capacity: " + String(static_cast<unsigned>(planet->GetEnergyReceptionCapacity())));

		colonizedText_->SetVisible(false);
	}
	else return;

	// planet image transfering
	planetImage_->SetTexture(LGeneralMethods::GetTextureFromNode(planetNode_));

	buildingManager_ = planetNode_->GetComponent<LBuildingsManager>();
	equipmentManager_ = controlNode_->GetComponent<LEquipmentManager>();
	if (!buildingManager_ || !equipmentManager_)
		return;

	uiSubStateRoot_->SetVisible(true);
	constructedBar_->SetVisible(true);
	uiSubStateRoot_->UpdateLayout();

	// buildings
	SubscribeToEvent(buildChoicesList_, E_ITEMSELECTED, URHO3D_HANDLER(LGameSpacePlanetState, HandleBuildItemSelected));
	SubscribeToEvent(completedBuildingsList_, E_ITEMSELECTED, URHO3D_HANDLER(LGameSpacePlanetState, HandleOpenBuildingInfo));

	// building
	SubscribeToEvent(improveBuildingButton_, E_PRESSED, URHO3D_HANDLER(LGameSpacePlanetState, HandleImproveBuilding));
	SubscribeToEvent(getResourceButton_, E_PRESSED, URHO3D_HANDLER(LGameSpacePlanetState, HandleTakeBuildingResourceDialog));

	// clouses
	SubscribeToEvent(closeBuildingButton_, E_PRESSED, URHO3D_HANDLER(LGameSpacePlanetState, HandleCloseBuilding));

	// update
	Scene* scene = parentState_->GetScene();
	if (scene)
		SubscribeToEvent(scene, E_SCENEUPDATE, URHO3D_HANDLER(LGameSpacePlanetState, HandleUpdate));

	SubscribeToEvent(backToSpace_, E_PRESSED, URHO3D_HANDLER(LGameSpacePlanetState, HandleBackToGameClick));
	SubscribeToEvent(showCargo_, E_PRESSED, URHO3D_HANDLER(LGameSpacePlanetState, HandleShowCargoClick));
}

void LGameSpacePlanetState::Create(Node* playerNode)
{
	controlNode_ = playerNode;

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/planet.xml");
	uiSubStateRoot_->LoadXML(layout->GetRoot(), style);
	uiSubStateRoot_->SetVisible(false);

	planetImage_ =                  static_cast<Sprite*>    (uiSubStateRoot_->GetChild("planetImage_", true));
	colonizedText_ =                static_cast<Text*>      (uiSubStateRoot_->GetChild("colonizedText_", true));
	planetTypeText_ =               static_cast<Text*>      (uiSubStateRoot_->GetChild("planetTypeText_", true));
	oreWealthText_ =                static_cast<Text*>      (uiSubStateRoot_->GetChild("oreWealthText_", true));
	energyReceptionCapacityText_ =  static_cast<Text*>      (uiSubStateRoot_->GetChild("energyReceptionCapacityText_", true));

	buildChoicesBar_ =                                       uiSubStateRoot_->GetChild("buildChoicesBar_", true);
	buildChoicesList_ =             static_cast<ListView*>  (uiSubStateRoot_->GetChild("buildChoicesList_", true));
	// TODO: planetInfoWindow Add info to planetDescBar

	constructedBar_ =                                        uiSubStateRoot_->GetChild("constructedBar_", true);
	completedBuildingsList_ =       static_cast<ListView*>  (uiSubStateRoot_->GetChild("completedBuildingsList_", true));

	buildingInfoBar_ =                                       uiSubStateRoot_->GetChild("buildingInfoBar_", true);
	buildingItemsList_ =            static_cast<ListView*>  (uiSubStateRoot_->GetChild("buildingItemsList_", true));
	improveBuildingButton_ =        static_cast<Button*>    (uiSubStateRoot_->GetChild("improveBuildingButton_", true));
	getResourceButton_ =            static_cast<Button*>    (uiSubStateRoot_->GetChild("getResourceButton_", true));
	closeBuildingButton_ =          static_cast<Button*>    (uiSubStateRoot_->GetChild("closeBuildingButton_", true));

	// cargo
	cargoElement_ =                                          uiSubStateRoot_->GetChild("cargoElement_", true);
	cargoList_ =                    static_cast<ListView*>  (uiSubStateRoot_->GetChild("cargoList_", true));

	// Bottom section
	backToSpace_ =                  static_cast<Button*>    (uiSubStateRoot_->GetChild("backToSpace_", true));
	showCargo_ =                    static_cast<Button*>    (uiSubStateRoot_->GetChild("showCargo_", true));
}

void LGameSpacePlanetState::HandleBuildItemSelected(StringHash eventType, VariantMap & eventData)
{
	U32 selection = eventData[ItemSelected::P_SELECTION].GetUInt();
	auto selectedBuilding = buildChoicesList_->GetItem(selection);

	VariantMap buildEventData;
	buildEventData[ObjectBuildEvent::PLAYER_ID] = controlNode_->GetID();
	buildEventData[ObjectBuildEvent::OBJECT_ID] = planetNode_->GetID();
	buildEventData[ObjectBuildEvent::BUILDING_NAME] = selectedBuilding->GetVar(buildingName).GetString();
	buildEventData[ObjectBuildEvent::TARGET_TYPE] = LBuilding::TargetType::TT_Planet;

	GetSubsystem<LSharedData>()->SendGameEvent(RE_OBJECT_BUILD, buildEventData);
}

void LGameSpacePlanetState::HandleOpenBuildingInfo(StringHash eventType, VariantMap & eventData)
{
	U32 index = eventData[ItemSelected::P_SELECTION].GetUInt();
	UIElement* building = completedBuildingsList_->GetItem(index);
	String bName = building->GetVar(buildingName).GetString();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	PrefabsStorage* prefabsStorage = GetSubsystem<PrefabsStorage>();

	constructedBar_->SetVisible(false);
	buildingInfoBar_->SetVisible(true);
	buildingInfoBar_->SetVar(buildingName, bName);

	buildingItemsList_->RemoveAllItems();
	if (bName == "radar")
	{
		getResourceButton_->SetVisible(true);
		getResourceButton_->SetVar(resource, LGameResource::ResourceType::Energy);
		improveBuildingButton_->SetVisible(true);
	}
	else if (bName == "metalExtractionFactory")
	{
		getResourceButton_->SetVisible(true);
		getResourceButton_->SetVar(resource, LGameResource::ResourceType::Metals);
	}
	else if (bName == "probesFactory")
	{
		UIElement* probesBar = new UIElement(context_);
		probesBar->SetLayout(LM_HORIZONTAL, 15);
		probesBar->SetStyleAuto();

		Sprite* probeSprite = probesBar->CreateChild<Sprite>();
		// TODO: index
		//probeSprite->SetTexture(buildings[0]->GetImage());
		probeSprite->SetFixedSize(70, 50);

		Text* probeName = probesBar->CreateChild<Text>();
		probeName->SetStyleAuto();
		probeName->SetText("Research probe");
		probeName->SetAlignment(HA_LEFT, VA_CENTER);

		Button* createProbeButton = probesBar->CreateChild<Button>();
		createProbeButton->SetTexture(cache->GetResource<Texture2D>("Textures/create.png"));
		createProbeButton->SetAlignment(HA_LEFT, VA_BOTTOM);
		createProbeButton->SetFixedSize(70, 30);

		auto cost = prefabsStorage->GetProbeCreateCost();
		LGeneralMethods::ShowCost(createProbeButton, controlNode_, cost.first_, cost.second_);

		buildingItemsList_->AddItem(probesBar);

		SubscribeToEvent(createProbeButton, E_PRESSED, URHO3D_HANDLER(LGameSpacePlanetState, HandleCreateProbe));
	}
	else if (bName == "weaponFactory")
	{
		ShowEquipment(LEquipment::ET_Weapon);
	}
	else if (bName == "engineFactory")
	{
		ShowEquipment(LEquipment::ET_Engine);
		ShowEquipment(LEquipment::ET_HyperDrive);
	}
	else if (bName == "shieldFactory")
	{
		ShowEquipment(LEquipment::ET_FieldGenerator);
	}
	else if (bName == "equipmentFactory")
	{
		ShowEquipment(LEquipment::ET_CrystalDetector);
		ShowEquipment(LEquipment::ET_RepairDrone);
	}
	else if (bName == "shipsFactory")
	{
		createShipButton_.Resize(Mob::MobType::MT_Count);
		for (U32 i = 0; i < Mob::MobType::MT_Count; i++)
		{
			ShipFactory* shipFactory = planetNode_->GetComponent<ShipFactory>(true);
			if (shipFactory && shipFactory->IsShipOnProduce(i))
				continue;

			UIElement* mobBar = new UIElement(context_);
			mobBar->SetLayout(LM_HORIZONTAL, 15);
			mobBar->SetStyleAuto();

			Sprite* mobSprite = mobBar->CreateChild<Sprite>();
			mobSprite->SetFixedSize(70, 50);

			Texture2D* texture = cache->GetResource<Texture2D>("Urho2D/ships/" + String(mobTypeNames[i]) + ".png");
			if (texture)
				mobSprite->SetTexture(texture);

			Text* shipName = mobBar->CreateChild<Text>();
			shipName->SetStyleAuto();
			// TODO
			shipName->SetText(String(mobTypeNames[i]));
			shipName->SetAlignment(HA_LEFT, VA_CENTER);

			createShipButton_[i] = mobBar->CreateChild<Button>();
			createShipButton_[i]->SetTexture(cache->GetResource<Texture2D>("Textures/create.png"));
			createShipButton_[i]->SetFixedSize(70, 25);
			createShipButton_[i]->SetAlignment(HA_LEFT, VA_CENTER);

			auto cost = prefabsStorage->GetMobCreateCost();
			//TODO : need to copy from tactical mode to legacy mode
			//LGeneralMethods::ShowCost(createShipButton_[i], controlNode_, cost.first_, cost.second_);

			buildingItemsList_->AddItem(mobBar);
			SubscribeToEvent(createShipButton_[i], E_PRESSED, URHO3D_HANDLER(LGameSpacePlanetState, HandleCreateShipClick));
		}
	}
	else if (bName == "orbitalFactory")
	{
		U32 count = 0;
		U32 size = 1;
		//TODO: HERE
		//unsigned size = SolidPlanetBuildingsManager::SolidPlanetOrbitalBuildingType::PO_Count;
		createOrbitalBuildingButton_.Resize(size);

		for (U32 i = 0; i < size; i++)
		{
			// TODO: hardcode TT_Star should be removed
			if (!prefabsStorage->IsBuildingAvailable(controlNode_, LBuilding::TT_Planet, "defenderPlatform"))
				continue;

			UIElement* orbitalBar = new UIElement(context_);
			orbitalBar->SetLayout(LM_HORIZONTAL, 15);
			orbitalBar->SetStyleAuto();

			Sprite* orbitalSprite = orbitalBar->CreateChild<Sprite>();
			orbitalSprite->SetFixedSize(70, 50);

			Texture2D* texture = cache->GetResource<Texture2D>("Urho2D/probes/probe.png");
			if (texture)
				orbitalSprite->SetTexture(texture);

			Text* orbitalBuildingName = orbitalBar->CreateChild<Text>();
			orbitalBuildingName->SetStyleAuto();
			// TODO
			orbitalBuildingName->SetText("defender platform");
			orbitalBuildingName->SetAlignment(HA_LEFT, VA_CENTER);

			createOrbitalBuildingButton_[i] = orbitalBar->CreateChild<Button>();
			createOrbitalBuildingButton_[i]->SetTexture(cache->GetResource<Texture2D>("Textures/create.png"));
			createOrbitalBuildingButton_[i]->SetFixedSize(70, 25);
			createOrbitalBuildingButton_[i]->SetAlignment(HA_LEFT, VA_CENTER);
			createOrbitalBuildingButton_[i]->SetVar(buildingName, "defenderPlatform");
			createOrbitalBuildingButton_[i]->SetVar("targetType", LBuilding::TT_Planet);

			auto cost = prefabsStorage->GetBuildingCreateCost(LBuilding::TT_Planet, "defenderPlatform");
			LGeneralMethods::ShowCost(createOrbitalBuildingButton_[i], controlNode_, cost.first_, cost.second_);

			buildingItemsList_->AddItem(orbitalBar);
			SubscribeToEvent(createOrbitalBuildingButton_[count], E_PRESSED,
				URHO3D_HANDLER(LGameSpacePlanetState, HandleCreateOrbitalBuilding));

			count++;
		}
	}
	else if (bName == "repairFactory")
	{
		UIElement* repairBar = new UIElement(context_);
		repairBar->SetLayout(LM_HORIZONTAL, 15);
		repairBar->SetStyleAuto();

		UIElement* repairImageBar = repairBar->CreateChild<UIElement>();
		repairImageBar->SetLayout(LM_VERTICAL, 15);
		repairImageBar->SetStyleAuto();

		Sprite* repairSprite = repairImageBar->CreateChild<Sprite>();
		repairSprite->SetTexture(cache->GetResource<Texture2D>("Urho2D/spaceships/ship1_11.png"));
		repairSprite->SetFixedSize(70, 50);

		repairEnergyButton_ = repairImageBar->CreateChild<Button>();
		repairEnergyButton_->SetStyleAuto();
		repairEnergyButton_->SetVerticalAlignment(VA_BOTTOM);

		Text* repairModuleName = repairBar->CreateChild<Text>();
		repairModuleName->SetStyleAuto();
		repairModuleName->SetText(String("Your spaceship"));
		repairModuleName->SetAlignment(HA_LEFT, VA_CENTER);

		repairEnergy_ = repairBar->CreateChild<Text>();
		repairEnergy_->SetStyleAuto();
		repairEnergy_->SetAlignment(HA_LEFT, VA_CENTER);

		repairButton_ = repairBar->CreateChild<Button>();
		repairButton_->SetTexture(cache->GetResource<Texture2D>("Textures/build.png"));
		repairButton_->SetAlignment(HA_LEFT, VA_CENTER);
		repairButton_->SetFixedSize(70, 30);

		buildingItemsList_->AddItem(repairBar);
		SubscribeToEvent(repairButton_, E_PRESSED, URHO3D_HANDLER(LGameSpacePlanetState, HandleRepair));
	}
}

void LGameSpacePlanetState::HandleTakeBuildingResourceDialog(StringHash eventType, VariantMap & eventData)
{
	Button* selectedButton = static_cast<Button*>(eventData[Pressed::P_ELEMENT].GetPtr());

	VariantMap takeResourceEventData;
	takeResourceEventData[GetBuildingResourceEnergyEvent::PLAYER_ID] = controlNode_->GetID();
	takeResourceEventData[GetBuildingResourceEnergyEvent::OBJECT_ID] = planetNode_->GetID();
	takeResourceEventData[GetBuildingResourceEnergyEvent::BUILDING_NAME] = selectedButton->GetVar(buildingName).GetString();
	takeResourceEventData[GetBuildingResourceEnergyEvent::RESOURCE_TYPE] = selectedButton->GetVar(resource).GetUInt();

	GetSubsystem<LSharedData>()->SendGameEvent(RE_GET_BUILDING_RESOURCE, takeResourceEventData);
}

void LGameSpacePlanetState::HandleImproveBuilding(StringHash eventType, VariantMap & eventData)
{
	VariantMap improveBuildingEventData;
	improveBuildingEventData[ImproveBuildingEvent::OBJECT_ID] = planetNode_->GetID();
	improveBuildingEventData[ImproveBuildingEvent::PLAYER_ID] = controlNode_->GetID();
	improveBuildingEventData[ImproveBuildingEvent::BUILDING_NAME] = improveBuildingButton_->GetVar(buildingName).GetString();

	GetSubsystem<LSharedData>()->SendGameEvent(RE_IMPROVE_BUILDING, improveBuildingEventData);
}

void LGameSpacePlanetState::HandleCreateProbe(StringHash eventType, VariantMap & eventData)
{
	VariantMap createProbeEventData;
	createProbeEventData[CreateProbeEvent::PLAYER_ID] = controlNode_->GetID();

	GetSubsystem<LSharedData>()->SendGameEvent(RE_CREATE_PROBE, createProbeEventData);
}

void LGameSpacePlanetState::HandleCreateEquipment(StringHash eventType, VariantMap & eventData)
{
	Button* selectedButton = static_cast<Button*>(eventData[Pressed::P_ELEMENT].GetPtr());

	VariantMap createEquipmentEventData;
	createEquipmentEventData[CreateEquipmentEvent::PLAYER_ID] = controlNode_->GetID();
	createEquipmentEventData[CreateEquipmentEvent::EQUIPMENT_TYPE] = selectedButton->GetVar(equipmentType).GetUInt();
	createEquipmentEventData[CreateEquipmentEvent::EQUIPMENT_NAME] = selectedButton->GetVar(equipmentName).GetString();

	GetSubsystem<LSharedData>()->SendGameEvent(RE_CREATE_EQUIPMENT, createEquipmentEventData);
}

void LGameSpacePlanetState::HandleCreateShipClick(StringHash eventType, VariantMap & eventData)
{
	// TODO: ptrs
	Button* selectedButton = static_cast<Button*>(eventData[Pressed::P_ELEMENT].GetPtr());
	for (U32 i = 0; i < createShipButton_.Size(); i++)
		if (createShipButton_[i] == selectedButton)
		{
			VariantMap productionMobsEventData;
			productionMobsEventData[ProductionMobsEvent::PLANET_ID] = planetNode_->GetID();
			productionMobsEventData[ProductionMobsEvent::MOB_NUMBER] = i;

			GetSubsystem<LSharedData>()->SendGameEvent(RE_MOBS_PRODUCTION, productionMobsEventData);
		}
}

void LGameSpacePlanetState::HandleCreateOrbitalBuilding(StringHash eventType, VariantMap & eventData)
{
	Button* selectedButton = static_cast<Button*>(eventData[Pressed::P_ELEMENT].GetPtr());

	VariantMap createBuildingEventData;
	createBuildingEventData[ObjectBuildEvent::OBJECT_ID] = planetNode_->GetID();
	createBuildingEventData[ObjectBuildEvent::PLAYER_ID] = controlNode_->GetID();
	createBuildingEventData[ObjectBuildEvent::BUILDING_NAME] = selectedButton->GetVar(buildingName);
	createBuildingEventData[ObjectBuildEvent::TARGET_TYPE] = selectedButton->GetVar(targetType).GetUInt();

	GetSubsystem<LSharedData>()->SendGameEvent(RE_OBJECT_BUILD, createBuildingEventData);
}

void LGameSpacePlanetState::HandleRepair(StringHash eventType, VariantMap & eventData)
{
	VariantMap RepairShipEventData;
	RepairShipEventData[RepairShipEvent::PLAYER_ID] = controlNode_->GetID();

	GetSubsystem<LSharedData>()->SendGameEvent(RE_SHIP_REPAIR, RepairShipEventData);
}

void LGameSpacePlanetState::HandleScrap(StringHash eventType, VariantMap & eventData)
{
	// TODO: ptrs
	Button* selectedButton = static_cast<Button*>(eventData[Pressed::P_ELEMENT].GetPtr());
	for (U32 i = 0; i < scrapButton_.Size(); i++)
		if (scrapButton_[i] == selectedButton)
		{
			VariantMap scrapEventData;
			scrapEventData[PutOnScrapEvent::PLAYER_ID] = controlNode_->GetID();
			scrapEventData[PutOnScrapEvent::CARGO_ELEMENT_ID] = cargoMapping_[selectedButton]->GetID();

			GetSubsystem<LSharedData>()->SendGameEvent(RE_PUT_ON_SCRAP, scrapEventData);
		}
}

void LGameSpacePlanetState::HandleCloseBuilding(StringHash eventType, VariantMap & eventData)
{
	buildingInfoBar_->SetVisible(false);
	constructedBar_->SetVisible(true);
}

void LGameSpacePlanetState::HandleBackToGameClick(StringHash eventType, VariantMap & eventData)
{
	Exit();
	parentState_->ExitSubstate();
}

void LGameSpacePlanetState::HandleShowCargoClick(StringHash eventType, VariantMap & eventData)
{
	cargoElement_->SetVisible(!cargoElement_->IsVisible());
}

void LGameSpacePlanetState::Exit()
{
	uiSubStateRoot_->SetVisible(false);
	cargoElement_->SetVisible(false);

	planetNode_.Reset();

	// network exceptions
	PODVector<StringHash> eventExceptions;
	eventExceptions.Push(RE_OBJECT_BUILD);

	UnsubscribeFromAllEventsExcept(eventExceptions, false);
}

void LGameSpacePlanetState::UpdateCompleteBuildings()
{
	completedBuildingsList_->RemoveAllItems();
	auto buildings = buildingManager_->GetCompletedBuildings();
	for (auto building : buildings)
	{
		UIElement* buildBar = new UIElement(context_);
		buildBar->SetLayout(LM_HORIZONTAL, 6);
		buildBar->SetStyleAuto();
		buildBar->SetVar(buildingName, building->GetName());

		Sprite* buildingImage = buildBar->CreateChild<Sprite>();
		buildingImage->SetTexture(LGeneralMethods::GetTextureFromNode(building));
		buildingImage->SetFixedSize(50, 30);

		Text* buildText = buildBar->CreateChild<Text>();
		buildText->SetVerticalAlignment(VA_CENTER);
		buildText->SetText(building->GetName());
		buildText->SetStyleAuto();

		completedBuildingsList_->AddItem(buildBar);
	}

	completedBuildingsList_->SetVisible(true);
}

void LGameSpacePlanetState::ShowEquipment(LEquipment::EquipmentType eType)
{
	static ResourceCache* cache = GetSubsystem<ResourceCache>();
	static PrefabsStorage* prefabs = GetSubsystem<PrefabsStorage>();
	auto availabeEquipment = prefabs->GetAvailabeLegacyEquipment(eType, controlNode_);

	for (auto& equipmentIt : availabeEquipment)
	{
		UIElement* equipmentBar = new UIElement(context_);
		equipmentBar->SetLayout(LM_HORIZONTAL, 15);
		equipmentBar->SetStyleAuto();

		Sprite* equipmentSprite = equipmentBar->CreateChild<Sprite>();
		equipmentSprite->SetFixedSize(70, 50);

		Texture2D* texture = LGeneralMethods::GetTextureFromNode(equipmentIt.second_);
		if (texture)
			equipmentSprite->SetTexture(texture);

		Text* weaponName = equipmentBar->CreateChild<Text>();
		weaponName->SetStyleAuto();
		// TODO
		weaponName->SetText(equipmentIt.first_);
		weaponName->SetAlignment(HA_LEFT, VA_CENTER);

		Button* createEquipmentButton = equipmentBar->CreateChild<Button>();
		createEquipmentButton->SetTexture(cache->GetResource<Texture2D>("Textures/create.png"));
		createEquipmentButton->SetFixedSize(70, 25);
		createEquipmentButton->SetAlignment(HA_LEFT, VA_CENTER);
		createEquipmentButton->SetVar(equipmentType, eType);
		createEquipmentButton->SetVar(equipmentName, equipmentIt.first_);

		auto cost = prefabs->GetLegacyEquipmentCreateCost(eType, equipmentIt.first_);
		LGeneralMethods::ShowCost(createEquipmentButton, controlNode_,
			cost.first_, cost.second_);

		buildingItemsList_->AddItem(equipmentBar);
		SubscribeToEvent(createEquipmentButton, E_PRESSED, URHO3D_HANDLER(LGameSpacePlanetState, HandleCreateEquipment));
	}
}

void LGameSpacePlanetState::UpdateProposalBuildings()
{
	if (!buildingManager_)
		return;

	buildChoicesList_->RemoveAllItems();
	auto buildingsProposal = buildingManager_->GetProposalBuildings(controlNode_);
	for (auto bName : buildingsProposal)
	{
		const Node* buildingPrefabNode = GetSubsystem<PrefabsStorage>()->GetBuilding(LBuilding::TT_Planet, buildingName);

		UIElement* buildingBar = new UIElement(context_);
		buildingBar->SetLayout(LM_HORIZONTAL, 6);
		buildingBar->SetStyleAuto();
		buildingBar->SetVar(buildingName, bName);

		Sprite* radioImage = buildingBar->CreateChild<Sprite>();
		radioImage->SetTexture(LGeneralMethods::GetTextureFromNode(buildingPrefabNode));
		radioImage->SetFixedSize(50, 30);
		radioImage->SetStyleAuto();

		// TODO: does not work
		LGeneralMethods::ShowCost(radioImage, controlNode_, LGameResource::ResourceType::Metals, 10000.f);

		Text* radioText = buildingBar->CreateChild<Text>();
		radioText->SetVerticalAlignment(VA_CENTER);

		radioText->SetText(buildingName);
		radioText->SetStyleAuto();

		buildChoicesList_->AddItem(buildingBar);
	}
}

void LGameSpacePlanetState::HandleUpdate(StringHash eventType, VariantMap & eventData)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	PrefabsStorage* prefabsStorage = GetSubsystem<PrefabsStorage>();

	LTeam* team = controlNode_->GetParentComponent<LTeam>(true);
	if (!team || !equipmentManager_ || !buildingManager_)
		return;

	if (planetNode_->GetComponent<Owner>())
		colonizedText_->SetVisible(true);
	else
		colonizedText_->SetVisible(false);

	// update complete buildings
	if (completedBuildingsList_->IsVisible())
		UpdateCompleteBuildings();

	// update proposal buildings
	if (buildChoicesList_->IsVisible())
		UpdateProposalBuildings();

	/* TODO: this should be removed?
	// radar isimprove update
	Building* radar = buildingManager_->GetBuilding("radar");
	if (radar && buildingManager_)
	{
		radarEnergyList_->SetVisible(false);
		getRadarEnergyButton_->SetVisible(false);
		improveRadarButton_->SetVisible(false);
	}
	*/

	isCargo_ = false;
	auto children = equipmentManager_->GetCargoElements();
	if (buildingInfoBar_->IsVisible())
	{
		String bName = buildingInfoBar_->GetVar(buildingName).GetString();

		if (bName == "radar")
		{
			LResourceManager* rManager = planetNode_->GetComponent<LResourceManager>();
			Text* radarEnergy = static_cast<Text*>(buildingItemsList_->GetItem(0));
			if (radarEnergy)
			{
				radarEnergy->SetText(String(static_cast<U32>(rManager->GetResourceValue(LGameResource::ResourceType::Energy))));
			}
		}
		else if (bName == "metalExtractionFactory")
		{
			LResourceManager* rManager = planetNode_->GetComponent<LResourceManager>();
			Text* metals = static_cast<Text*>(buildingItemsList_->GetItem(0));
			if (metals)
			{
				metals->SetText(String(static_cast<U32>(rManager->GetResourceValue(LGameResource::ResourceType::Metals))));
			}
		}
		else if (bName == "weaponFactory")
		{
			// cargo  weapon update
			isCargo_ = true;
			cargoList_->RemoveAllItems();

			for (U32 i = 0; i < children.Size(); i++)
			{
				Weapon* weapon = children[i]->GetDerivedComponent<Weapon>(true);
				if (!weapon)
					continue;

				UIElement* elementBar = new UIElement(context_);
				elementBar->SetLayout(LM_HORIZONTAL, 15);
				elementBar->SetStyleAuto();

				UIElement* cargoElement = elementBar->CreateChild<UIElement>();
				//cargoElement->SetLayout(LM_FREE, 10, IntRect(10, 10, 10, 10));
				cargoElement->SetAlignment(HA_LEFT, VA_TOP);
				cargoElement->SetFixedSize(70, 70);
				cargoElement->SetHorizontalAlignment(HA_LEFT);

				Sprite* cargoCellSprite = cargoElement->CreateChild<Sprite>();
				cargoCellSprite->SetFixedSize(70, 70);

				Texture2D* cellCargoTexture = cache->GetResource<Texture2D>("Textures/cell.png");
				cargoCellSprite->SetTexture(cellCargoTexture);

				Sprite* cargoSprite = cargoElement->CreateChild<Sprite>();
				cargoSprite->SetFixedSize(70, 50);

				StaticSprite2D* sprtiteCargo = children[i]->GetComponent<StaticSprite2D>();
				if (!sprtiteCargo)
					continue;

				Texture2D* cargoTexture = sprtiteCargo->GetSprite()->GetTexture();
				if (cargoTexture)
					cargoSprite->SetTexture(cargoTexture);

				Text* elementName = elementBar->CreateChild<Text>();
				elementName->SetStyleAuto();
				elementName->SetAlignment(HA_LEFT, VA_CENTER);
				elementName->SetText(weapon->GetNode()->GetName());

				cargoList_->AddItem(elementBar);
			}
		}
		else if (bName == "engineFactory")
		{
			// cargo engine update
			isCargo_ = true;
			cargoList_->RemoveAllItems();

			for (U32 i = 0; i < children.Size(); i++)
			{
				ShipEngine* engine = children[i]->GetComponent<ShipEngine>(true);
				HyperDrive* hyperEngine = children[i]->GetComponent<HyperDrive>(true);
				if (!engine && !hyperEngine)
					continue;

				UIElement* elementBar = new UIElement(context_);
				elementBar->SetLayout(LM_HORIZONTAL, 15);
				elementBar->SetStyleAuto();

				UIElement* cargoElement = elementBar->CreateChild<UIElement>();
				//cargoElement->SetLayout(LM_FREE, 10, IntRect(10, 10, 10, 10));
				cargoElement->SetAlignment(HA_LEFT, VA_TOP);
				cargoElement->SetFixedSize(70, 70);
				cargoElement->SetHorizontalAlignment(HA_LEFT);

				Sprite* cargoCellSprite = cargoElement->CreateChild<Sprite>();
				cargoCellSprite->SetFixedSize(70, 70);

				Texture2D* cellCargoTexture = cache->GetResource<Texture2D>("Textures/cell.png");
				cargoCellSprite->SetTexture(cellCargoTexture);

				Sprite* cargoSprite = cargoElement->CreateChild<Sprite>();
				cargoSprite->SetFixedSize(70, 50);

				StaticSprite2D* sprtiteCargo = children[i]->GetComponent<StaticSprite2D>();
				if (!sprtiteCargo)
					continue;

				Texture2D* cargoTexture = sprtiteCargo->GetSprite()->GetTexture();
				if (cargoTexture)
					cargoSprite->SetTexture(cargoTexture);

				Text* elementName = elementBar->CreateChild<Text>();
				elementName->SetStyleAuto();
				elementName->SetAlignment(HA_LEFT, VA_CENTER);

				String name = "unidentified engine";
				if (engine)
					name = engineTypeNames[engine->GetEngineType()];
				if (hyperEngine)
					name = hyperEngineTypeNames[hyperEngine->GetEngineType()];
				elementName->SetText(name);

				cargoList_->AddItem(elementBar);
			}
		}
		else if (bName == "shieldFactory")
		{
			// cargo shield update
			isCargo_ = true;
			cargoList_->RemoveAllItems();

			for (U32 i = 0; i < children.Size(); i++)
			{
				EnergyFieldGenerator* generator = children[i]->GetComponent<EnergyFieldGenerator>(true);
				if (!generator)
					continue;

				UIElement* elementBar = new UIElement(context_);
				elementBar->SetLayout(LM_HORIZONTAL, 15);
				elementBar->SetStyleAuto();

				UIElement* cargoElement = elementBar->CreateChild<UIElement>();
				//cargoElement->SetLayout(LM_FREE, 10, IntRect(10, 10, 10, 10));
				cargoElement->SetAlignment(HA_LEFT, VA_TOP);
				cargoElement->SetFixedSize(70, 70);
				cargoElement->SetHorizontalAlignment(HA_LEFT);

				Sprite* cargoCellSprite = cargoElement->CreateChild<Sprite>();
				cargoCellSprite->SetFixedSize(70, 70);

				Texture2D* cellCargoTexture = cache->GetResource<Texture2D>("Textures/cell.png");
				cargoCellSprite->SetTexture(cellCargoTexture);

				Sprite* cargoSprite = cargoElement->CreateChild<Sprite>();
				cargoSprite->SetFixedSize(70, 50);

				StaticSprite2D* sprtiteCargo = children[i]->GetComponent<StaticSprite2D>();
				if (!sprtiteCargo)
					continue;

				Texture2D* cargoTexture = sprtiteCargo->GetSprite()->GetTexture();
				if (cargoTexture)
					cargoSprite->SetTexture(cargoTexture);

				Text* elementName = elementBar->CreateChild<Text>();
				elementName->SetStyleAuto();
				elementName->SetAlignment(HA_LEFT, VA_CENTER);
				elementName->SetText(String(energyFieldGeneratorTypeNames[generator->GetGeneratorType()]));

				cargoList_->AddItem(elementBar);
			}
		}
		else if (bName == "eqiupmentFactory")
		{
			// cargo equipment update
			isCargo_ = true;
			cargoList_->RemoveAllItems();

			for (U32 i = 0; i < children.Size(); i++)
			{
				CrystalDetector* crystalDetector = children[i]->GetComponent<CrystalDetector>(true);
				RepairDrone* drone = children[i]->GetComponent<RepairDrone>(true);
				if (!crystalDetector && !drone)
					continue;

				UIElement* elementBar = new UIElement(context_);
				elementBar->SetLayout(LM_HORIZONTAL, 15);
				elementBar->SetStyleAuto();

				UIElement* cargoElement = elementBar->CreateChild<UIElement>();
				//cargoElement->SetLayout(LM_FREE, 10, IntRect(10, 10, 10, 10));
				cargoElement->SetAlignment(HA_LEFT, VA_TOP);
				cargoElement->SetFixedSize(70, 70);
				cargoElement->SetHorizontalAlignment(HA_LEFT);

				Sprite* cargoCellSprite = cargoElement->CreateChild<Sprite>();
				cargoCellSprite->SetFixedSize(70, 70);

				Texture2D* cellCargoTexture = cache->GetResource<Texture2D>("Textures/cell.png");
				cargoCellSprite->SetTexture(cellCargoTexture);

				Sprite* cargoSprite = cargoElement->CreateChild<Sprite>();
				cargoSprite->SetFixedSize(70, 50);

				StaticSprite2D* sprtiteCargo = children[i]->GetComponent<StaticSprite2D>();
				if (!sprtiteCargo)
					continue;

				Texture2D* cargoTexture = sprtiteCargo->GetSprite()->GetTexture();
				if (cargoTexture)
					cargoSprite->SetTexture(cargoTexture);

				String elemName = "unidentified element";

				if (crystalDetector && prefabsStorage->IsLegacyEquipmentAvailable(controlNode_, LEquipment::EquipmentType::ET_CrystalDetector, crystalDetector->GetNode()->GetName()))
					elemName = "Crystal detector";
				else if (drone && prefabsStorage->IsLegacyEquipmentAvailable(controlNode_, LEquipment::EquipmentType::ET_RepairDrone, drone->GetNode()->GetName()))
					elemName = "Repair Drone";

				Text* elementName = elementBar->CreateChild<Text>();
				elementName->SetStyleAuto();
				elementName->SetAlignment(HA_LEFT, VA_CENTER);
				elementName->SetText(elemName);
				cargoList_->AddItem(elementBar);
			}
		}
		else if (bName == "shipFactory")
		{
			for (U32 i = 0; i < createShipButton_.Size(); i++)
			{
				ShipFactory* shipFactory = dynamic_cast<ShipFactory*>(buildingManager_->GetBuilding("shipFactory"));
				if (shipFactory && shipFactory->IsShipOnProduce(i) && createShipButton_[i])
					createShipButton_[i]->SetVisible(false);
			}
		}
		else if (bName == "repairFactory")
		{
			LDurability* shipDurability = controlNode_->GetComponent<LDurability>();
			if (shipDurability)
			{
				F32 durability = shipDurability->GetDurability();
				F32 maxDurability = shipDurability->GetMaxDurability();

				repairEnergyButton_->SetFixedSize(static_cast<S32>(70 * Min(1.0f, logf(durability))), 5);
				// TODO: threshold
				if (durability < 15.f)
					repairEnergyButton_->SetColor(Color::RED);
				else
					repairEnergyButton_->SetColor(Color::GREEN);

				repairEnergy_->SetText(String(static_cast<S32>(durability)) + "/" + String(static_cast<S32>(maxDurability)));

				if (static_cast<S32>(durability) == static_cast<S32>(maxDurability))
					repairButton_->SetVisible(false);
			}
		}
		else if (bName == "scrapMetalFactory")
		{
			isCargo_ = true;

			scrapButton_.Resize(children.Size());
			for (U32 i = 0; i < children.Size(); i++)
			{
				UIElement* elementBar = new UIElement(context_);
				elementBar->SetLayout(LM_HORIZONTAL, 15);
				elementBar->SetStyleAuto();

				UIElement* cargoElement = elementBar->CreateChild<UIElement>();
				//cargoElement->SetLayout(LM_FREE, 10, IntRect(10, 10, 10, 10));
				cargoElement->SetAlignment(HA_LEFT, VA_TOP);
				cargoElement->SetFixedSize(70, 70);
				cargoElement->SetHorizontalAlignment(HA_LEFT);

				Sprite* cargoCellSprite = cargoElement->CreateChild<Sprite>();
				cargoCellSprite->SetFixedSize(70, 70);

				Texture2D* cellCargoTexture = cache->GetResource<Texture2D>("Textures/cell.png");
				cargoCellSprite->SetTexture(cellCargoTexture);

				Sprite* cargoSprite = cargoElement->CreateChild<Sprite>();
				cargoSprite->SetFixedSize(70, 50);

				StaticSprite2D* sprtiteCargo = children[i]->GetComponent<StaticSprite2D>();
				if (!sprtiteCargo)
					continue;

				Texture2D* cargoTexture = sprtiteCargo->GetSprite()->GetTexture();
				if (cargoTexture)
					cargoSprite->SetTexture(cargoTexture);

				Text* elementName = elementBar->CreateChild<Text>();
				elementName->SetStyleAuto();

				Weapon* weapon = children[i]->GetDerivedComponent<Weapon>(true);
				if (weapon)
					elementName->SetText(weapon->GetNode()->GetName());

				if (children[i]->GetComponent<RepairDrone>(true))
					elementName->SetText("Repair drone");

				if (children[i]->GetComponent<CrystalDetector>(true))
					elementName->SetText("Crystal detector");

				if (children[i]->GetComponent<HyperDrive>(true))
					elementName->SetText("Hyper engine");

				ShipEngine* engine = children[i]->GetComponent<ShipEngine>(true);
				if (engine)
					elementName->SetText(String(engineTypeNames[engine->GetEngineType()]) + " engine");

				scrapButton_[i] = elementBar->CreateChild<Button>();
				scrapButton_[i]->SetTexture(cache->GetResource<Texture2D>("Textures/scrap.png"));
				scrapButton_[i]->SetAlignment(HA_LEFT, VA_CENTER);
				scrapButton_[i]->SetFixedSize(70, 30);

				cargoMapping_[scrapButton_[i]] = children[i];

				buildingItemsList_->AddItem(elementBar);

				SubscribeToEvent(scrapButton_[i], E_PRESSED,
					URHO3D_HANDLER(LGameSpacePlanetState, HandleScrap));
			}

		}
	}

	if (isCargo_)
		cargoElement_->SetVisible(true);
};
