#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Network/Network.h>

#include "legacyMode/gameEvents/lGameEvents.h"
#include "legacyMode/utility/lSharedData.h"
#include "legacyMode/utility/lGeneralMethods.h"

#include "legacyMode/playerLogic/lResearchTree.h"

void LResearchTree::RegisterObject(Context* context)
{
	context->RegisterFactory<LResearchTree>();

	URHO3D_MIXED_ACCESSOR_ATTRIBUTE("researchedItems", GetResearchedItemsAttr, SetResearchedItemsAttr,
		PODVector<U8>, Variant::emptyBuffer, AM_DEFAULT);
}

LResearchTree::LResearchTree(Context * context) :
	LogicComponent(context)
{
	Network* network = GetSubsystem<Network>();
	if (!network->GetServerConnection())
		SetUpdateEventMask(USE_UPDATE);
	else
		SetUpdateEventMask(0);

	using namespace Research;

	allResearchItems_[Base] =                           new ResearchItem("Base", 0);
	allResearchItems_[Base]->isCompleted_ = true;

	allResearchItems_[ElectromagneticFields] =			new ResearchItem("ElectromagneticFields",	10);
	allResearchItems_[Lasers] =							new ResearchItem("Lasers",					10);
	allResearchItems_[EnergyShielding] =				new ResearchItem("EnergyShielding",			10);
	allResearchItems_[EnergyElements] =					new ResearchItem("EnergyElements",			10);
	allResearchItems_[StrongEMFields] =					new ResearchItem("StrongEMFields",			10);
	allResearchItems_[Plasma] =							new ResearchItem("Plasma",					10);
	allResearchItems_[QuantumPhysics] =					new ResearchItem("QuantumPhysics",			10);
	allResearchItems_[PhaseDetectors] =					new ResearchItem("PhaseDetectors",			10);
	allResearchItems_[SuperlightTravel] =				new ResearchItem("SuperlightTravel",		10);

	// TODO buildings
	allResearchItems_[TODO_EngineFactory] = new ResearchItem("TODO_EngineFactory", 10);
	allResearchItems_[TODO_EquipmentFactory] = new ResearchItem("TODO_EquipmentFactory", 10);
	allResearchItems_[TODO_MetalExtractionFactory] = new ResearchItem("TODO_MetalExtractionFactory", 10);
	allResearchItems_[TODO_OrbitalFactory] = new ResearchItem("TODO_OrbitalFactory", 10);
	allResearchItems_[TODO_ProbesFactory] = new ResearchItem("TODO_ProbesFactory", 10);
	allResearchItems_[TODO_RepairFactory] = new ResearchItem("TODO_RepairFactory", 10);
	allResearchItems_[TODO_ScrapMetalFactory] = new ResearchItem("TODO_ScrapMetalFactory", 10);
	allResearchItems_[TODO_ShipFactory] = new ResearchItem("TODO_ShipFactory", 10);
	allResearchItems_[TODO_WeaponFactory] = new ResearchItem("TODO_WeaponFactory", 10);
	allResearchItems_[TODO_ScienceLaboratory] = new ResearchItem("TODO_ScienceLaboratory", 10);
	allResearchItems_[TODO_DefenderPlatform] = new ResearchItem("TODO_DefenderPlatform", 10);

	// TODO equipment
	allResearchItems_[TODO_repairDrone] = new ResearchItem("TODO_repairDrone", 10);

	// TODO weapon
	allResearchItems_[TODO_chemicalWeapon] = new ResearchItem("TODO_chemicalWeapon", 10);
	allResearchItems_[TODO_railgunWeapon] = new ResearchItem("TODO_railgunWeapon", 10);
	allResearchItems_[TODO_rockets] = new ResearchItem("TODO_rockets", 10);

	// TODO energy field generators
	allResearchItems_[TODO_chemicalFieldGenerators] = new ResearchItem("TODO_chemicalFieldGenerators", 10);
	allResearchItems_[TODO_ionFieldGenerators] = new ResearchItem("TODO_ionFieldGenerators", 10);
	allResearchItems_[TODO_nuclearFieldGenerators] = new ResearchItem("TODO_nuclearFieldGenerators", 10);
	allResearchItems_[TODO_photonFieldGenerators] = new ResearchItem("TODO_photonFieldGenerators", 10);
	allResearchItems_[TODO_plasmaFieldGenerators] = new ResearchItem("TODO_plasmaFieldGenerators", 10);
	allResearchItems_[TODO_sunFieldGenerators] = new ResearchItem("TODO_sunFieldGenerators", 10);

	// TODO hyper drives
	allResearchItems_[TODO_chemicalHyperEngine] = new ResearchItem("TODO_chemicalHyperEngine", 10);
	allResearchItems_[TODO_ionHyperEngine] = new ResearchItem("TODO_ionHyperEngine", 10);
	allResearchItems_[TODO_nuclearHyperEngine] = new ResearchItem("TODO_nuclearHyperEngine", 10);
	allResearchItems_[TODO_photonHyperEngine] = new ResearchItem("TODO_photonHyperEngine", 10);
	allResearchItems_[TODO_plasmaHyperEngine] = new ResearchItem("TODO_plasmaHyperEngine", 10);
	allResearchItems_[TODO_sunHyperEngine] = new ResearchItem("TODO_sunHyperEngine", 10);

	// TODO engines
	allResearchItems_[TODO_chemicalShipEngine] = new ResearchItem("TODO_chemicalShipEngine", 10);
	allResearchItems_[TODO_nuclearShipEngine] = new ResearchItem("TODO_nuclearHyperEngine", 10);
	allResearchItems_[TODO_photonShipEngine] = new ResearchItem("TODO_photonShipEngine", 10);
	allResearchItems_[TODO_plasmaShipEngine] = new ResearchItem("TODO_plasmaShipEngine", 10);
	allResearchItems_[TODO_sunShipEngine] = new ResearchItem("TODO_sunShipEngine", 10);

	// ElectromagneticFields
	allResearchItems_[Lasers]->prerequisiteItems_.Insert(ElectromagneticFields);
	allResearchItems_[EnergyShielding]->prerequisiteItems_.Insert(ElectromagneticFields);
	allResearchItems_[EnergyElements]->prerequisiteItems_.Insert(ElectromagneticFields);
	allResearchItems_[StrongEMFields]->prerequisiteItems_.Insert(ElectromagneticFields);

	// StrongEMFields
	allResearchItems_[Plasma]->prerequisiteItems_.Insert(StrongEMFields);

	// QuantumPhysics
	allResearchItems_[PhaseDetectors]->prerequisiteItems_.Insert(QuantumPhysics);
	allResearchItems_[SuperlightTravel]->prerequisiteItems_.Insert(QuantumPhysics);
}

LResearchTree::~LResearchTree()
{
}

LResearchTree::ResearchItem::ResearchItem(const String & name, F32 baseCost)
	: name_(name), baseCost_(baseCost), isInProcess_(false), isCompleted_(false)
{
}

void LResearchTree::SetResearchedItemsAttr(const PODVector<U8>& value)
{
	if (value.Empty())
		return;

	MemoryBuffer buffer(value);
	while (!buffer.IsEof())
	{
		ResearchItem* researchItem = new ResearchItem();

		Research::ResearchType type = static_cast<Research::ResearchType>(buffer.ReadUInt());
		researchItem->name_ = buffer.ReadString();
		researchItem->baseCost_ = buffer.ReadFloat();
		researchItem->isCompleted_ = buffer.ReadBool();

		U32 prereqSize = buffer.ReadUInt();
		for (U32 i = 0; i < prereqSize; i++)
		{
			researchItem->prerequisiteItems_.Insert(static_cast<Research::ResearchType>(buffer.ReadUInt()));
		}

		allResearchItems_[type] = researchItem;
		MarkNetworkUpdate();
	}
}

PODVector<U8> LResearchTree::GetResearchedItemsAttr() const
{
	VectorBuffer ret;

	for (auto researchItemIt = allResearchItems_.Begin(); researchItemIt != allResearchItems_.End(); ++researchItemIt)
	{
		ret.WriteUInt(researchItemIt->first_);
		ret.WriteString(researchItemIt->second_->name_);
		ret.WriteFloat(researchItemIt->second_->baseCost_);
		ret.WriteBool(researchItemIt->second_->isCompleted_);

		ret.WriteUInt(researchItemIt->second_->prerequisiteItems_.Size());
		for (auto prereqIt = researchItemIt->second_->prerequisiteItems_.Begin();
			prereqIt != researchItemIt->second_->prerequisiteItems_.End(); ++prereqIt)
		{
			ret.WriteUInt(*prereqIt);
		}
	}

	return ret.GetBuffer();
}

bool LResearchTree::PrerequisitesReady(U32 researchItemType)
{
	auto researchInQuestion = allResearchItems_[researchItemType];

	for (auto prereqIt = researchInQuestion->prerequisiteItems_.Begin(); prereqIt != researchInQuestion->prerequisiteItems_.End(); ++prereqIt)
	{
		if (!allResearchItems_[*prereqIt]->isCompleted_)
			return false;
	}

	return true;
}

HashSet<U32> LResearchTree::GetAvailableResearches()
{
	HashSet<U32> result;

	for (auto researchItemIt = allResearchItems_.Begin(); researchItemIt != allResearchItems_.End(); ++researchItemIt)
		if (!researchItemIt->second_->isCompleted_ && PrerequisitesReady(researchItemIt->first_))
			result.Insert(researchItemIt->first_);

	return result;
}

HashSet<U32> LResearchTree::GetCompletedResearches() const
{
	HashSet<U32> result;

	for (auto researchItemIt = allResearchItems_.Begin(); researchItemIt != allResearchItems_.End(); ++researchItemIt)
		if (researchItemIt->second_->isCompleted_)
			result.Insert(researchItemIt->first_);

	return result;
}

void LResearchTree::StartResearch(U32 researchItemType)
{
	if (!PrerequisitesReady(researchItemType))
		return;

	allResearchItems_[researchItemType]->isInProcess_ = true;
	researchTimer_.Reset();
	MarkNetworkUpdate();
}

F32 LResearchTree::GetResearchCost(U32 researchItemType)
{
	return allResearchItems_[researchItemType]->baseCost_;
}

String LResearchTree::GetResearchName(U32 researchItemType)
{
	return allResearchItems_[researchItemType]->name_;
}

void LResearchTree::Update(F32 timeStep)
{
	for (U32 i = 0; i < allResearchItems_.Size(); i++)
		if (allResearchItems_[i]->isInProcess_ && researchTimer_.GetMSec(false) > 100.f * allResearchItems_[i]->baseCost_)
		{
			allResearchItems_[i]->isInProcess_ = false;
			allResearchItems_[i]->isCompleted_ = true;

			S32 teamNumber = LGeneralMethods::GetTeamNumber(node_);

			VariantMap eventData;
			eventData[ResearchCompletedEvent::TEAM_NUMBER] = teamNumber;
			eventData[ResearchCompletedEvent::RESEARCH_TYPE] = i;

			LSharedData* sharedData = GetSubsystem<LSharedData>();
			sharedData->SendServerToTeamPlayersEvent(RE_RESEARCH_COMPLETED, teamNumber, eventData);

			MarkNetworkUpdate();

			break;
		}
}
