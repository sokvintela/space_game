#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Scene/Scene.h>

#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/playerLogic/lTeam.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/galaxy/spaceObjects/lDebris.h"
#include "legacyMode/galaxy/lStarSystem.h"
#include "legacyMode/managers/lDataManager.h"
#include "legacyMode/spaceship/lDurability.h"

#include "legacyMode/space/lFindObjectsHelper.h"

LFindObjectsHelper::LFindObjectsHelper(Context* context, Node* controlNode, UIElement* uiSpaceState)
	: Object(context), 
	controlNode_(controlNode),
	enteredNode_(nullptr),
	hoveringNode_(nullptr)
{
}

LFindObjectsHelper::~LFindObjectsHelper()
{
}

void LFindObjectsHelper::FindClosestObjects(UIElement* objectWindow, Vector2 mousePos)
{
	objectWindow_ = objectWindow;
	if (!objectWindow_)
		return;

	Text* ownerText =			static_cast<Text*>  (objectWindow_->GetChild("objectOwner_", true));
	typeText_ =					static_cast<Text*>  (objectWindow_->GetChild("objectType_", true));
	Text* durabilityText =		static_cast<Text*>  (objectWindow_->GetChild("objectDurability_", true));
	Text* efficiencyText =		static_cast<Text*>  (objectWindow_->GetChild("objectEfficiency_", true));

	Vector<Text*> lines;
	lines.Push(static_cast<Text*>  (objectWindow_->GetChild("firstText_", true)));

	Button* toPlanetButton = static_cast<Button*>(objectWindow_->GetChild("toPlanetButton_", true));

	if (!controlNode_)
		return;

	Scene* scene = controlNode_->GetScene();
	if (!scene)
		return;

	SetOffShipFromObject<LDataManager>(10.f, mousePos, 20.f);

	PODVector<Node*> starSystemNodes;
	scene->GetChildrenWithComponent<LStarSystem>(starSystemNodes, true);
	for (auto systemNode : starSystemNodes)
		FindClosestObject<LDataManager>(systemNode, mousePos, 100.f, 10.f, 5.f);

	if (enteredNode_)
	{
		LDebris* debris = enteredNode_->GetComponent<LDebris>();
		if (debris)
		{
			LResourceManager* rManager = controlNode_->GetComponent<LResourceManager>();
			if (rManager)
				rManager->ChangeResourceValue(LGameResource::ResourceType::Metals, debris->Getmetals());

			debris->GetNode()->Remove();
			hoveringNode_ = enteredNode_ = nullptr;
		}
	}

	ownerText->SetVisible(false);
	typeText_->SetVisible(false);
	durabilityText->SetVisible(false);
	efficiencyText->SetVisible(false);

	for (U32 i = 0; i < lines.Size(); i++)
		lines[i]->SetVisible(false);

	toPlanetButton->SetVisible(false);

	if (hoveringNode_)
	{
		LDataManager* dManager = hoveringNode_->GetComponent<LDataManager>();
		if (dManager)
		{
			String objectName = dManager->GetObjectName(controlNode_);

			// team
			if (dManager->GetTeamNumber() != -1)
			{
				ownerText->SetVisible(true);
				ownerText->SetText("Team : " + String(dManager->GetTeamNumber()));
			}

			// type
			typeText_->SetVisible(true);
			typeText_->SetText(objectName);

			// durability
			LDurability* durability = hoveringNode_->GetComponent<LDurability>();
			if (durability)
			{
				durabilityText->SetVisible(true);
				durabilityText->SetText(String(static_cast<U32>(durability->GetDurability())) + " / " + String(static_cast<U32>(durability->GetMaxDurability())));
			}

			// efficiency
			if (dManager->HasEfficiency())
			{
				efficiencyText->SetVisible(true);
				efficiencyText->SetText(objectName + " efficiency: " + String(static_cast<U32>(dManager->GetObjectEfficiency())));
			}

			// lines
			for (U32 i = 0; i < dManager->infoWindow_.Size(); i++)
			{
				lines[i]->SetVisible(true);
				lines[i]->SetText(dManager->infoWindow_[i]);
			}
		}

		// land
		LPlanet* planet = hoveringNode_->GetComponent<LPlanet>();
		if (planet)
		{
			S32 playerTeamNumber = LGeneralMethods::GetTeamNumber(controlNode_);
			U32 ownerTeamNumber = LGeneralMethods::GetOwnerTeamNumber(hoveringNode_);
			if (ownerTeamNumber == LTeam::NoTeamId || (ownerTeamNumber == playerTeamNumber))
				toPlanetButton->SetVisible(true);
		}
	}
}

Node* LFindObjectsHelper::GetEnteredNode() const
{
	return enteredNode_;
}

Node* LFindObjectsHelper::GetHoveringNode() const
{
	return hoveringNode_;
}

void LFindObjectsHelper::SetEnteredNode(Node * enteredNode)
{
	enteredNode_ = enteredNode;
}
