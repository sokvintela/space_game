#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Node.h>

#include "tacticalMode/gameEvents/tGameEvents.h"
//#include "legacyMode/managers/lBuildingsManager.h"
#include "utility/generalMethods.h"
#include "utility/sharedData.h"
#include "prefabs/prefabsStorage.h"
//#include "legacyMode/buildings/lBuilding.h"

#include "strategicMode/sSpaceBuildingsUI.h"

SSpaceBuildings::SSpaceBuildings(Context* context, Node* controlNode, UIElement* uiSpaceState)
	: Object(context)
	, controlNode_(controlNode)
{
	// building info
	buildingWindow_ = uiSpaceState->GetChild("buildingWindow_", true);

	// proposal buildings
	proposalBuildingsWindow_ = uiSpaceState->GetChild("proposalBuildings_", true);

	// completed buildings
	completedBuildingsWindow_ = uiSpaceState->GetChild("completedBuildings_", true);
}

SSpaceBuildings::~SSpaceBuildings()
{
}

void SSpaceBuildings::UpdateBuildingsUI(Node* enteredNode)
{
	UpdateProposalBuildingsUI(enteredNode);
	UpdateCompletedBuildingsUI(enteredNode);
}

void SSpaceBuildings::UpdateProposalBuildingsUI(Node* enteredNode)
{
	// buildings
	if (!enteredNode)
		return;

	enteredNode_ = enteredNode;

	proposalBuildingsWindow_->RemoveAllChildren();
	proposalBuildingsWindow_->SetVisible(true);

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	if (!controlNode_)
		return;

	/*
	LBuildingsManager* buildingManager = enteredNode_->GetComponent<LBuildingsManager>();
	if (!buildingManager)
		return;
		
	auto buildingsProposalIds = buildingManager->GetProposalBuildings(controlNode_);

	PrefabsStorage* prefabs = GetSubsystem<PrefabsStorage>();
	// proposal
	auto it = buildingsProposalIds.Begin();
	for (U32 i = 0; i < 10; i++)
	{
		if (it == buildingsProposalIds.End())
			break;

		UIElement* buildingsRow = proposalBuildingsWindow_->CreateChild<UIElement>();
		buildingsRow->SetFixedHeight(50);
		buildingsRow->SetLayout(LM_HORIZONTAL);
		buildingsRow->SetAlignment(HA_RIGHT, VA_TOP);
		buildingsRow->SetStyleAuto();

		for (U32 j = 0; j < 4; j++)
		{
			if (it == buildingsProposalIds.End())
				break;

			String buildingName = *it;
			LBuilding::TargetType tType = GeneralMethods::GetObjectTargetType(enteredNode_);

			auto buildingPrefab = prefabs->GetBuilding(tType, buildingName);

			Button* proposalBuildButton = buildingsRow->CreateChild<Button>();
			proposalBuildButton->SetTexture(GeneralMethods::GetTextureFromNode(buildingPrefab));
			proposalBuildButton->SetBlendMode(BLEND_ADD);
			proposalBuildButton->SetFixedSize(30, 30);
			proposalBuildButton->SetVar("buildingName", buildingName);

			// cost
			auto cost = prefabs->GetBuildingCreateCost(tType, buildingName);

			GeneralMethods::ShowCost(proposalBuildButton, controlNode_, cost.first_, cost.second_);

			SubscribeToEvent(proposalBuildButton, E_PRESSED, URHO3D_HANDLER(SSpaceBuildings, HandleObjectBuild));

			++it;
		}
	}
	*/
}

void SSpaceBuildings::UpdateCompletedBuildingsUI(Node* enteredNode)
{
	// buildings
	if (!enteredNode)
		return;

	enteredNode_ = enteredNode;

	completedBuildingsWindow_->RemoveAllChildren();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	if (!controlNode_)
		return;
	/*
	LBuildingsManager* buildingManager = enteredNode_->GetComponent<LBuildingsManager>();
	if (!buildingManager)
		return;

	auto completedBuildings = buildingManager->GetCompletedBuildings();

	// completed
	U32 completedCount = completedBuildings.Size();
	U32 count = 0;
	for (U32 i = 0; i < 12; i++)
	{
		if (count == completedCount)
			break;

		UIElement* buildingsRow = completedBuildingsWindow_->CreateChild<UIElement>();
		buildingsRow->SetFixedHeight(50);
		buildingsRow->SetLayout(LM_HORIZONTAL);
		buildingsRow->SetAlignment(HA_RIGHT, VA_TOP);
		buildingsRow->SetStyleAuto();

		for (U32 j = 0; j < 4; j++)
		{
			if (count == completedCount)
				break;

			String buildingName = completedBuildings[count]->GetName();

			Button* completedBuildButton = new Button(context_);
			completedBuildButton->SetTexture(GeneralMethods::GetTextureFromNode(completedBuildings[count]));
			completedBuildButton->SetBlendMode(BLEND_ADD);
			completedBuildButton->SetFixedSize(30, 30);
			completedBuildButton->SetVar("buildingName", buildingName);
			buildingsRow->AddChild(completedBuildButton);

			// TODO: does not work
			ToolTip* buttonToolTip = completedBuildButton->CreateChild<ToolTip>();
			buttonToolTip->SetStyleAuto();
			Text* buildingText = buttonToolTip->CreateChild<Text>();
			buildingText->SetStyleAuto();
			buildingText->SetText(buildingName);
			buildingText->SetAlignment(HA_LEFT, VA_BOTTOM);

			TPlanet* planet = enteredNode_->GetComponent<TPlanet>();
			if (planet && planet->GetPlanetType() == LPlanet::PlanetType::PT_Solid)
				SubscribeToEvent(completedBuildButton, E_PRESSED, URHO3D_HANDLER(SSpaceBuildings, HandlePlanetOpen));

			count++;
		}
	}

	if (proposalBuildingsWindow_->GetChildren().Size() == 0)
		proposalBuildingsWindow_->SetVisible(false);
		*/
}

void SSpaceBuildings::SwitchOff()
{
	proposalBuildingsWindow_->SetVisible(false);
}

void SSpaceBuildings::HandleObjectBuild(StringHash eventType, VariantMap & eventData)
{
	/*
	// TODO: distance
	if (!controlNode_ || (controlNode_->GetWorldPosition2D() - enteredNode_->GetWorldPosition2D()).Length() > 30.f)
		return;

	VariantMap buildEventData;

	buildEventData[ObjectBuildEvent::PLAYER_ID] = controlNode_->GetID();
	buildEventData[ObjectBuildEvent::OBJECT_ID] = enteredNode_->GetID();

	Button* button = static_cast<Button*>(eventData[Pressed::P_ELEMENT].GetPtr());
	buildEventData[ObjectBuildEvent::BUILDING_NAME] = button->GetVar("buildingName").GetString();
	buildEventData[ObjectBuildEvent::TARGET_TYPE] = GeneralMethods::GetObjectTargetType(enteredNode_);

	GetSubsystem<LSharedData>()->SendGameEvent(RE_OBJECT_BUILD, buildEventData);

	UpdateBuildingsUI(enteredNode_);
	*/
}

void SSpaceBuildings::HandlePlanetOpen(StringHash eventType, VariantMap& eventData)
{
	/*
	SendEvent(RE_PLANET_OPEN);
	*/
}
