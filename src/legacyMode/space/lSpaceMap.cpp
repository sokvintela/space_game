#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Core/Context.h>

#include "legacyMode/utility/lSharedData.h"
#include "legacyMode/playerLogic/owner.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/utility/lGeneralMethods.h"
#include "legacyMode/gameEvents/lGameEvents.h"
#include "legacyMode/managers/lDataManager.h"
#include "legacyMode/playerLogic/lTeam.h"
#include "legacyMode/galaxy/galaxy.h"

#include "legacyMode/space/lSpaceMap.h"

static const F32 mathPi = static_cast<F32>(M_PI);

using namespace Urho3D;

void LGameSpaceMapState::RegisterEvents(Context * context)
{
	Network* network = context->GetSubsystem<Network>();
	network->RegisterRemoteEvent(RE_HYPER_DRIVE);
}

LGameSpaceMapState::LGameSpaceMapState(Urho3D::Context * context, UIElement* uiSubStateRoot_) :
	ISubState(context, uiSubStateRoot_),
	gRadius_(0.f)
{
}

void LGameSpaceMapState::Create(Node* playerNode)
{
	playerNode_ = playerNode;

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/spaceMap.xml");
	uiSubStateRoot_->LoadXML(layout->GetRoot(), style);
	uiSubStateRoot_->SetVisible(false);

	window_ =		static_cast<Window*>(uiSubStateRoot_->GetChild("window_", true));
	backButton_ =	static_cast<Button*>(uiSubStateRoot_->GetChild("backButton_", true));

	Galaxy* galaxy = playerNode_->GetParentComponent<Galaxy>(true);
	if (!galaxy)
		return;

	gRadius_ = galaxy->galaxyRadius_;

	IntVector2 rootSize = uiSubStateRoot_->GetSize();

	// star systems
	PODVector<Node*> starSystemNodes = LGeneralMethods::GetStarSystemNodes(playerNode_);
	for (U32 i = 0; i < starSystemNodes.Size(); i++)
	{
		// system pos real
		Vector2 gamePos = starSystemNodes[i]->GetPosition2D();
		F32 mapPosX = (gRadius_ + gamePos.x_) * window_->GetWidth() / (2 * gRadius_);
		F32 mapPosY = (gRadius_ - gamePos.y_) * window_->GetHeight() / (2 * gRadius_);

		// system pos map
		IntVector2 elementSize = IntVector2(50, 50);
		IntVector2 mapPos = IntVector2(mapPosX, mapPosY) - elementSize / 2.f;

		StarSystemMapInfo starSystemMapInfo;
		starSystemMapInfo.starSystemButton_ = window_->CreateChild<Button>();
		starSystemMapInfo.starSystemButton_->SetPriority(0);
		starSystemMapInfo.starSystemButton_->SetColor(Color::TRANSPARENT);
		starSystemMapInfo.starSystemButton_->SetName("starSystem" + String(i));
		starSystemMapInfo.starSystemButton_->SetSize(elementSize);
		starSystemMapInfo.starSystemButton_->SetPosition(mapPos);
		starSystemMapInfo.starSystemButton_->SetVisible(false);

		ToolTip* systemToolTip = starSystemMapInfo.starSystemButton_->CreateChild<ToolTip>();
		systemToolTip->SetStyleAuto();
		starSystemMapInfo.jumpCost_ = systemToolTip->CreateChild<Text>();
		starSystemMapInfo.jumpCost_->SetStyleAuto();
		starSystemMapInfo.jumpCost_->SetFont(starSystemMapInfo.jumpCost_->GetFont(), 20);

		elementSize = IntVector2(rootSize / 20);
		mapPos = IntVector2(mapPosX, mapPosY) - elementSize / 2.f;
		starSystemMapInfo.sunSprite_ = window_->CreateChild<Sprite>();
		starSystemMapInfo.sunSprite_->SetSize(elementSize);
		starSystemMapInfo.sunSprite_->SetPosition(mapPos.x_, mapPos.y_);
		starSystemMapInfo.sunSprite_->SetVisible(false);

		PODVector<Node*> planetNodes = LGeneralMethods::GetPlanetNodes(starSystemNodes[i]);
		for (U32 j = 0; j < planetNodes.Size(); j++)
		{
			StarSystemMapInfo::PlanetMapInfo planetMapInfo;
			planetMapInfo.planetSprite_ = window_->CreateChild<Sprite>();
			planetMapInfo.planetSprite_->SetTexture(cache->GetResource<Texture2D>("Urho2D/planets/Planet15.png"));
			planetMapInfo.planetSprite_->SetSize(rootSize / 40);
			planetMapInfo.planetSprite_->SetVisible(false);

			LPlanet* planet = planetNodes[j]->GetComponent<LPlanet>();
			if (planet && planet->GetPlanetType() == LPlanet::PlanetType::PT_Solid)
			{
				planetMapInfo.raceText_ = window_->CreateChild<Text>();
				planetMapInfo.raceText_->SetPriority(0);
				planetMapInfo.raceText_->SetStyleAuto();
				planetMapInfo.raceText_->SetPosition(mapPosX, mapPosY);
				planetMapInfo.raceText_->SetName("raceText");
				planetMapInfo.raceText_->SetText(Races::GetRaceName(j));// TODO: sharedData_->shipsRaces_[planet->GetOwnerId()]));
				planetMapInfo.raceText_->SetColor(Color::BLACK);
				planetMapInfo.raceText_->SetVisible(false);
			}

			U32 planetID = planetNodes[j]->GetID();
			starSystemMapInfo.planets_[planetID] = planetMapInfo;
		}

		U32 starSystemID = starSystemNodes[i]->GetID();
		starSystems_[starSystemID] = starSystemMapInfo;
	}

	Texture2D* myTexture = cache->GetResource<Texture2D>("Urho2D/map/me.png");
	mySprite_ = window_->CreateChild<Sprite>();
	mySprite_->SetSize(rootSize / 15);
	mySprite_->SetTexture(myTexture);
	mySprite_->SetOpacity(0.5f);
}

void LGameSpaceMapState::Enter(Node* playerNode)
{
	uiSubStateRoot_->SetVisible(true);
	uiSubStateRoot_->UpdateLayout();

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	LResourceManager* rManager = playerNode->GetComponent<LResourceManager>();
	LTeam* team = playerNode->GetParentComponent<LTeam>(true);
	if (!rManager || !team)
		return;

	// current star system
	S32 currentSystemID = -1;
	LDataManager* dManager = playerNode->GetComponent<LDataManager>();
	if (dManager)
		currentSystemID = dManager->GetCurrentStarSystemID();

	// my pos real
	Vector2 myPos = playerNode->GetWorldPosition2D();
	F32 myMapPosX = (gRadius_ + myPos.x_) * window_->GetWidth() / (2.f * gRadius_);
	F32 myMapPosY = (gRadius_ - myPos.y_) * window_->GetHeight() / (2.f * gRadius_);

	// my pos map
	Vector2 myMapPos = Vector2(myMapPosX, myMapPosY);
	mySprite_->SetPosition(myMapPos - Vector2(mySprite_->GetSize().x_ / 2.f, mySprite_->GetSize().y_ / 2.f));

	// ways
	for (auto wayIt = ways_.Begin(); wayIt != ways_.End(); ++wayIt)
		wayIt->sprite_->Remove();

	ways_.Clear();

	// star systems
	for (auto starSystemIt = starSystems_.Begin(); starSystemIt != starSystems_.End(); ++starSystemIt)
	{
		U32 starSystemID = starSystemIt->first_;
		auto& starSystemInfo = starSystemIt->second_;

		if (!team->IsStarSystemFound(starSystemID))
			continue;
		else
		{
			// found
			starSystemInfo.sunSprite_->SetTexture(cache->GetResource<Texture2D>("Urho2D/map/question.png"));
			starSystemInfo.sunSprite_->SetVisible(true);
			starSystemInfo.starSystemButton_->SetVisible(true);

			SubscribeToEvent(starSystemInfo.starSystemButton_, E_PRESSED, URHO3D_HANDLER(LGameSpaceMapState, StarSystemHyperDrive));

			// jump cost
			Node* starSystemNode = LGeneralMethods::GetObjectNode(playerNode, starSystemID);
			F32 distance = (starSystemNode->GetWorldPosition2D() - myPos).Length();
			starSystemInfo.jumpCost_->SetText(String(static_cast<U32>(distance)));
			starSystemInfo.jumpCost_->SetColor(Color::RED);

			if (rManager->HasResource(LGameResource::Energy, distance))
				starSystemInfo.jumpCost_->SetColor(Color::GREEN);

			// way
			if (starSystemID != currentSystemID)
			{
				Vector2 beginPos = mySprite_->GetPosition() + Vector2(mySprite_->GetSize().x_ / 2.f, mySprite_->GetSize().y_ / 2.f);
				Sprite* starSystemSprite = starSystems_[starSystemID].sunSprite_;
				Vector2 targetPos = starSystemSprite->GetPosition() + Vector2(starSystemSprite->GetSize().x_ / 2.f, starSystemSprite->GetSize().y_ / 2.f);

				Vector2 direction = targetPos - beginPos;
				direction.Normalize();

				F32 radianAngle = std::acos(direction.y_ / direction.Length());
				F32 degreeAngle = radianAngle * 180.f / mathPi;

				if (beginPos.x_ <= targetPos.x_)
					degreeAngle = -degreeAngle;

				WayMapInfo wayMapInfo;
				wayMapInfo.direction_ = direction;
				wayMapInfo.targetPos_ = targetPos;

				wayMapInfo.sprite_ = window_->CreateChild<Sprite>();
				wayMapInfo.sprite_->SetSize(uiSubStateRoot_->GetSize() / 240);
				wayMapInfo.sprite_->SetTexture(cache->GetResource<Texture2D>("Textures/laser.png"));
				wayMapInfo.sprite_->SetPosition(beginPos);
				wayMapInfo.sprite_->SetRotation(degreeAngle + 90.f);

				ways_.Push(wayMapInfo);
			}

			if (!team->IsStarSystemSurveyed(starSystemID))
				continue;

			// surveyed
			starSystemInfo.sunSprite_->SetTexture(cache->GetResource<Texture2D>("Urho2D/sources/star.png"));
			StaticSprite2D* starSprite = starSystemNode->GetComponent<StaticSprite2D>(true);
			if (starSprite)
				starSystemInfo.sunSprite_->SetColor(starSprite->GetColor());

			// planets
			for (auto planetIt = starSystemInfo.planets_.Begin(); planetIt != starSystemInfo.planets_.End(); ++planetIt)
			{
				U32 planetID = planetIt->first_;
				auto& planetInfo = planetIt->second_;

				planetInfo.planetSprite_->SetVisible(true);

				Owner* owner = LGeneralMethods::GetOwner(playerNode, planetID);
				if (!owner)
					continue;

				if (owner->GetTeamNumber() == team->GetTeamNumber())
					planetInfo.planetSprite_->SetTexture(cache->GetResource<Texture2D>("Urho2D/planet18.png"));
				else
					planetInfo.planetSprite_->SetColor(Color::CYAN);

				if (team->IsPlanetSurveyed(starSystemID, planetID))
					planetInfo.raceText_->SetVisible(true);
			}
		}
	}

	SubscribeToEvent(backButton_, E_PRESSED, URHO3D_HANDLER(LGameSpaceMapState, HandleBackClick));
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(LGameSpaceMapState, HandleUpdate));
}

void LGameSpaceMapState::StarSystemHyperDrive(StringHash eventType, VariantMap & eventData)
{
	if (!playerNode_)
		return;

	// TODO: ptrs
	Button* selectedButton = static_cast<Button*>(eventData[Pressed::P_ELEMENT].GetPtr());
	for (auto starSystemIt = starSystems_.Begin(); starSystemIt != starSystems_.End(); ++starSystemIt)
	{
		if (starSystemIt->second_.starSystemButton_ == selectedButton)
		{
			VariantMap hyperDriveEventData;
			hyperDriveEventData[StarSystemDriveEvent::PLAYER_ID] = playerNode_;
			hyperDriveEventData[StarSystemDriveEvent::STAR_SYSTEM_ID] = starSystemIt->first_;

			GetSubsystem<LSharedData>()->SendGameEvent(RE_HYPER_DRIVE, hyperDriveEventData);

			Exit();
			parentState_->ExitSubstate();
		}
	}
}

void LGameSpaceMapState::HandleBackClick(StringHash eventType, VariantMap & eventData)
{
	Exit();
	parentState_->ExitSubstate();
}

void LGameSpaceMapState::HandleUpdate(StringHash eventType, VariantMap & eventData)
{
	// planets
	for (auto starSystemIt = starSystems_.Begin(); starSystemIt != starSystems_.End(); ++starSystemIt)
	{
		U32 starSystemID = starSystemIt->first_;
		Node* starSystemNode = LGeneralMethods::GetObjectNode(playerNode_, starSystemID);

		for (auto planetIt = starSystemIt->second_.planets_.Begin(); planetIt != starSystemIt->second_.planets_.End(); ++planetIt)
		{
			U32 planetID = planetIt->first_;
			Node* planetNode = LGeneralMethods::GetObjectNode(playerNode_, planetID);

			Vector2 gamePos = starSystemNode->GetPosition2D() + 10 * planetNode->GetPosition2D();
			F32 fmapPosX = (gRadius_ + gamePos.x_) * window_->GetWidth() / (2 * gRadius_);
			F32 fmapPosY = (gRadius_ - gamePos.y_) * window_->GetHeight() / (2 * gRadius_);
			planetIt->second_.planetSprite_->SetPosition(fmapPosX, fmapPosY);
		}
	}

	// ways
	for (auto wayIt = ways_.Begin(); wayIt != ways_.End(); ++wayIt)
	{
		F32 currentAlfa = wayIt->currentAlpha_;
		if (currentAlfa < 0)
			currentAlfa = 0.f;

		if (wayIt->currentAlpha_ == 0)
			continue;

		Sprite* waySprite = wayIt->sprite_;
		if (!waySprite)
			continue;

		Vector2 targetPos = wayIt->targetPos_;
		Vector2 currentPos = waySprite->GetPosition();
		IntVector2 waySize = waySprite->GetSize();
		if ((targetPos - currentPos).Length() <= (waySize.x_ + 3))
			continue;

		Vector2 direction = wayIt->direction_;
		waySprite->SetPosition(currentPos + 2 * direction * 0.00001f);
		waySprite->SetSize(waySize.x_ + 6.0f, 5.0f);

		wayIt->sprite_->SetOpacity(currentAlfa);
		wayIt->currentAlpha_ -= 0.00001f;
	}
}

void LGameSpaceMapState::Exit()
{
	UnsubscribeFromAllEvents();
	uiSubStateRoot_->SetVisible(false);
}
