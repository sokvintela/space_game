#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Scene/Scene.h>

#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/playerLogic/tTeam.h"
#include "tacticalMode/starSystem/tDebris.h"
#include "utility/generalMethods.h"
#include "tacticalMode/starSystem/tStarSystem.h"
#include "tacticalMode/starSystem/tDebris.h"
#include "tacticalMode/managers/tDataManager.h"
#include "tacticalMode/spaceship/tDurability.h"
#include "tacticalMode/starSystem/tPlanet.h"
#include "tacticalMode/starSystem/tBaseShip.h"

#include "tacticalMode/space/tFindObjectsHelper.h"

#include <limits>

TFindObjectsHelper::TFindObjectsHelper(Context* context, Node* controlNode, UIElement* uiObjectWindow)
	: Object(context), 
	controlNode_(controlNode),
	enteredNode_(nullptr),
	hoveringNode_(nullptr)
{
	if (!uiObjectWindow)
		return;

	objectWindow_ = uiObjectWindow;

	ownerText_ =      static_cast<Text*>  (objectWindow_->GetChild("objectOwner_", true));
	typeText_ =       static_cast<Text*>  (objectWindow_->GetChild("objectType_", true));
	durabilityText_ = static_cast<Text*>  (objectWindow_->GetChild("objectDurability_", true));
	efficiencyText_ = static_cast<Text*>  (objectWindow_->GetChild("objectEfficiency_", true));
	resourcesText_ =  static_cast<Text*>  (objectWindow_->GetChild("objectResources_", true));
}

TFindObjectsHelper::~TFindObjectsHelper()
{
}

void TFindObjectsHelper::FindClosestObjects(Vector2 mousePos, bool& closeToBase)
{
	static F32 operabilityRadius = 20.f;
	static F32 mouseHoveringArea = 40.f;
	static F32 visibilityRadius = 1000.f;

	if (!controlNode_)
		return;

	// fly away from the object
	if (enteredNode_ && enteredNode_->HasComponent<TDataManager>())
	{
		F32 distanceToObject = (enteredNode_->GetWorldPosition2D() - controlNode_->GetWorldPosition2D()).Length();
		if (distanceToObject > operabilityRadius)
			enteredNode_ = nullptr;
	}

	if (hoveringNode_ && hoveringNode_->HasComponent<TDataManager>())
	{
		F32 distanceToObject = (mousePos - controlNode_->GetWorldPosition2D()).Length();
		if (distanceToObject > mouseHoveringArea)
		{
			hoveringNode_ = nullptr;
			objectWindow_->SetVisible(false);
		}
	}

	// collect all the information objects
	Vector<Node*> objectNodes;

	TStarSystem* system = GeneralMethods::GetOnlyComponent<TStarSystem>(controlNode_);
	if (!system)
		return;

	objectNodes.Push(GeneralMethods::GetObjectNode(controlNode_, system->starID_));

	for (U32 asteroidID : system->asteroidIDs_)
		objectNodes.Push(GeneralMethods::GetObjectNode(controlNode_, asteroidID));
	
	for (U32 planetID : system->planetIDs_)
		objectNodes.Push(GeneralMethods::GetObjectNode(controlNode_, planetID));

	for (U32 teamID : system->teamIDs_)
	{
		TTeam* team = GeneralMethods::GetComponent<TTeam>(controlNode_, teamID);
		if (!team)
			continue;

		objectNodes.Push(GeneralMethods::GetObjectNode(controlNode_, team->baseShipID_));

		for (U32 playerID : team->playerIDs_)
			objectNodes.Push(GeneralMethods::GetObjectNode(controlNode_, playerID));

		for (U32 mobID : team->mobIDs_)
			objectNodes.Push(GeneralMethods::GetObjectNode(controlNode_, mobID));
	}

	// choose the closest from myself and from my mouse objects
	F32 minHoveringDistance = std::numeric_limits<F32>::infinity();
	F32 minEnteredDistance = std::numeric_limits<F32>::infinity();

	for (auto objectNode : objectNodes)
	{
		if (!objectNode)
			continue;

		F32 distanceToObject = (objectNode->GetWorldPosition2D() - controlNode_->GetWorldPosition2D()).Length();
		if (distanceToObject < visibilityRadius)
		{
			F32 mouseDistance = (objectNode->GetWorldPosition2D() - mousePos).Length();
			if (mouseDistance < mouseHoveringArea)
			{
				if (mouseDistance < minHoveringDistance)
					hoveringNode_ = objectNode;
			}

			if (objectNode == controlNode_)
				continue;

			if (distanceToObject < operabilityRadius)
			{
				if (distanceToObject < minEnteredDistance)
					enteredNode_ = objectNode;
			}
		}
	}

	// if debris or base ship near myself
	if (enteredNode_)
	{
		TDebris* tDebris = enteredNode_->GetComponent<TDebris>();
		if (tDebris)
		{
			TResourceManager* playerRManager = controlNode_->GetComponent<TResourceManager>();
			TResourceManager* debrisRManager = tDebris->GetComponent<TResourceManager>();
			if (playerRManager && debrisRManager)
				playerRManager->ChangeResourceValue(TGameResource::ResourceType::Metals, debrisRManager->GetResourceValue(TGameResource::ResourceType::Metals));

			tDebris->GetNode()->Remove();
			hoveringNode_ = enteredNode_ = nullptr;
		}
		else
		{
			if (enteredNode_->HasComponent<TBaseShip>() 
				&& GeneralMethods::IsFriendlyTeam(enteredNode_))
			{
				closeToBase = true;
			}
			else
			{
				closeToBase = false;
			}
		}
	}

	// show closes object information
	ownerText_->SetVisible(false);
	typeText_->SetVisible(false);
	durabilityText_->SetVisible(false);
	efficiencyText_->SetVisible(false);
	resourcesText_->SetVisible(false);

	if (hoveringNode_)
	{
		TDataManager* dManager = hoveringNode_->GetComponent<TDataManager>();
		if (dManager)
		{
			// team
			if (dManager->GetTeamNumber() != -1)
			{
				ownerText_->SetVisible(true);
				ownerText_->SetText("Team : " + String(dManager->GetTeamNumber()));
			}

			// type
			typeText_->SetVisible(true);
			typeText_->SetText(dManager->GetObjectName());
			typeText_->SetColor(GeneralMethods::IsFriendlyTeam(hoveringNode_, controlNode_) ? Color::GREEN : Color::RED);

			// durability
			TDurability* durability = hoveringNode_->GetComponent<TDurability>();
			if (durability)
			{
				durabilityText_->SetVisible(true);
				durabilityText_->SetText("Durability " + String(static_cast<U32>(durability->GetDurability())) + " / " + String(static_cast<U32>(durability->GetMaxDurability())));
			}

			// efficiency
			if (dManager->HasEfficiency())
			{
				efficiencyText_->SetVisible(true);
				efficiencyText_->SetText("Efficiency: " + String(static_cast<U32>(dManager->GetObjectEfficiency())));
			}

			TResourceManager* resources = hoveringNode_->GetComponent<TResourceManager>();
			if (resources)
			{
				resourcesText_->SetVisible(true);
				String totalResourcesStr;

				auto allResources = resources->GetAllResources();
				for (auto res : allResources)
				{
					totalResourcesStr += res->GetResourceName();
					totalResourcesStr += ": ";
					totalResourcesStr += res->GetResourceValue();
					totalResourcesStr += "  ";
				}

				resourcesText_->SetText(totalResourcesStr);
			}
		}
	}
}

Node* TFindObjectsHelper::GetEnteredNode() const
{
	return enteredNode_;
}

Node* TFindObjectsHelper::GetHoveringNode() const
{
	return hoveringNode_;
}

void TFindObjectsHelper::SetEnteredNode(Node * enteredNode)
{
	enteredNode_ = enteredNode;
}
