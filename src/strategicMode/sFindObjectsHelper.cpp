#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Scene/Scene.h>

#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/playerLogic/tTeam.h"
#include "utility/generalMethods.h"
#include "tacticalMode/starSystem/tDebris.h"
#include "tacticalMode/starSystem/tStarSystem.h"
#include "tacticalMode/managers/tDataManager.h"
#include "tacticalMode/spaceship/tDurability.h"

#include "strategicMode/sFindObjectsHelper.h"

SFindObjectsHelper::SFindObjectsHelper(Context* context, Node* controlNode, UIElement* uiSpaceState)
	: Object(context), 
	controlNode_(controlNode),
	enteredNode_(nullptr),
	hoveringNode_(nullptr)
{
}

SFindObjectsHelper::~SFindObjectsHelper()
{
}

void SFindObjectsHelper::FindClosestObjects(UIElement* objectWindow, Vector2 mousePos)
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

	SetOffShipFromObject<TDataManager>(10.f, mousePos, 20.f);

	PODVector<Node*> starSystemNodes;
	scene->GetChildrenWithComponent<TStarSystem>(starSystemNodes, true);
	for (auto systemNode : starSystemNodes)
		FindClosestObject<TDataManager>(systemNode, mousePos, 100.f, 10.f, 5.f);

	if (enteredNode_)
	{
		TDebris* debris = enteredNode_->GetComponent<TDebris>();
		if (debris)
		{
			TResourceManager* rManager = controlNode_->GetComponent<TResourceManager>();
			/*
			if (rManager)
				rManager->ChangeResourceValue(TGameResource::ResourceType::Metals, debris->Getmetals());
				*/
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
		TDataManager* dManager = hoveringNode_->GetComponent<TDataManager>();
		if (dManager)
		{
			//String objectName = dManager->GetObjectName(controlNode_);

			// team
			if (dManager->GetTeamNumber() != -1)
			{
				ownerText->SetVisible(true);
				ownerText->SetText("Team : " + String(dManager->GetTeamNumber()));
			}

			// type
			typeText_->SetVisible(true);
			//typeText_->SetText(objectName);

			// durability
			TDurability* durability = hoveringNode_->GetComponent<TDurability>();
			if (durability)
			{
				durabilityText->SetVisible(true);
				durabilityText->SetText(String(static_cast<U32>(durability->GetDurability())) + " / " + String(static_cast<U32>(durability->GetMaxDurability())));
			}

			// efficiency
			if (dManager->HasEfficiency())
			{
				efficiencyText->SetVisible(true);
				//efficiencyText->SetText(objectName + " efficiency: " + String(static_cast<U32>(dManager->GetObjectEfficiency())));
			}

			// lines
			for (U32 i = 0; i < dManager->infoWindow_.Size(); i++)
			{
				lines[i]->SetVisible(true);
				lines[i]->SetText(dManager->infoWindow_[i]);
			}
		}

		// land
		TPlanet* planet = hoveringNode_->GetComponent<TPlanet>();
		if (planet)
		{
			S32 playerTeamNumber = GeneralMethods::GetTeamNumber(controlNode_);
			/*
			U32 ownerTeamNumber = GeneralMethods::GetOwnerTeamNumber(hoveringNode_);
			if (ownerTeamNumber == TTeam::NoTeamId || (ownerTeamNumber == playerTeamNumber))
				toPlanetButton->SetVisible(true);
				*/
		}
	}
}

Node* SFindObjectsHelper::GetEnteredNode() const
{
	return enteredNode_;
}

Node* SFindObjectsHelper::GetHoveringNode() const
{
	return hoveringNode_;
}

void SFindObjectsHelper::SetEnteredNode(Node * enteredNode)
{
	enteredNode_ = enteredNode;
}
