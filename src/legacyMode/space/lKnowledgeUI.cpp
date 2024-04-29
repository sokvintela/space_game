#include <Urho3D/Scene/Node.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>

#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/playerLogic/lResearchTree.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "legacyMode/utility/lSharedData.h"
#include "legacyMode/utility/lGeneralMethods.h"

#include "legacyMode/space/lKnowledgeUI.h"

LKnowledgeUI::LKnowledgeUI(Context* context, Node* controlNode, UIElement* uiSpaceState)
	: Object(context)
	, controlNode_(controlNode)
{
	completedResearchList_ = static_cast<ListView*>    (uiSpaceState->GetChild("completedResearchList_", true));
	availableResearchList_ = static_cast<ListView*>    (uiSpaceState->GetChild("availableResearchList_", true));

	HandleUpdateKnowledges();
}

LKnowledgeUI::~LKnowledgeUI()
{
}

void LKnowledgeUI::HandleUpdateKnowledges()
{
	Node* teamNode = LGeneralMethods::GetMyTeamNode(controlNode_);
	if (!teamNode)
		return;

	LResourceManager* resourceManager = teamNode->GetComponent<LResourceManager>();
	LResearchTree* researchTree = teamNode->GetComponent<LResearchTree>();
	if (!resourceManager || !researchTree)
		return;

	ResourceCache* cache = GetSubsystem<ResourceCache>();

	HashSet<U32> proposal = researchTree->GetAvailableResearches();
	availableResearchList_->RemoveAllItems();

	for (auto researchIt = proposal.Begin(); researchIt != proposal.End(); ++researchIt)
	{
		Button* researchItem = new Button(context_);

		XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
		XMLFile* layout = cache->GetResource<XMLFile>("UI/parts/researchItem.xml");
		researchItem->LoadXML(layout->GetRoot(), style);
		researchItem->SetVar("Research", *researchIt);

		Text* researchName = static_cast<Text*>(researchItem->GetChild("researchName", true));
		researchName->SetText(researchTree->GetResearchName(*researchIt));

		Text* tooltipText = static_cast<Text*>(researchItem->GetChild("tooltipText", true));
		tooltipText->SetText(String(static_cast<U32>(researchTree->GetResearchCost(*researchIt))));

		if (resourceManager->HasResource(LGameResource::ResourceType::Knowledge, researchTree->GetResearchCost(*researchIt)))
			researchName->SetColor(Color::GREEN);
		else
			researchName->SetColor(Color::GRAY);

		availableResearchList_->AddItem(researchItem);

		SubscribeToEvent(researchItem, E_PRESSED, URHO3D_HANDLER(LKnowledgeUI, HandleSelectKnowledge));
	}

	HashSet<U32> completed = researchTree->GetCompletedResearches();
	completedResearchList_->RemoveAllItems();
	for (auto researchIt = completed.Begin(); researchIt != completed.End(); ++researchIt)
	{
		Text* researchName = new Text(context_);
		researchName->SetStyleAuto();
		researchName->SetText(researchTree->GetResearchName(*researchIt));
		researchName->SetAlignment(HA_LEFT, VA_CENTER);

		completedResearchList_->AddItem(researchName);
	}
}

void LKnowledgeUI::SwitchKnowledgeUI() const
{
	availableResearchList_->SetVisible(!availableResearchList_->IsVisible());
	completedResearchList_->SetVisible(!completedResearchList_->IsVisible());
}

void LKnowledgeUI::HandleSelectKnowledge(StringHash eventType, VariantMap & eventData)
{
	Button* selectedButton = static_cast<Button*>(eventData[Pressed::P_ELEMENT].GetPtr());
	U32 researchType = selectedButton->GetVar("Research").GetUInt();

	Node* teamNode = LGeneralMethods::GetMyTeamNode(controlNode_);
	if (!teamNode)
		return;

	LResourceManager* resourceManager = teamNode->GetComponent<LResourceManager>();
	LResearchTree* researchTree = teamNode->GetComponent<LResearchTree>();

	VariantMap researchSelectionEventData;

	S32 teamNumber = LGeneralMethods::GetTeamNumber(controlNode_);
	researchSelectionEventData[ResearchSelectionEvent::TEAM_NUMBER] = teamNumber;
	researchSelectionEventData[ResearchSelectionEvent::RESEARCH_TYPE] = researchType;

	if (resourceManager && researchTree &&
		resourceManager->HasResource(LGameResource::ResourceType::Knowledge, researchTree->GetResearchCost(researchType)))
	{
		GetSubsystem<LSharedData>()->SendGameEvent(RE_RESEARCH_SELECTION, researchSelectionEventData);
	}

	availableResearchList_->SetVisible(false);
	completedResearchList_->SetVisible(false);
}
