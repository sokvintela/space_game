#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/IO/Log.h>

#include "ai/AINodeStorage.h"
#include "ai/AILogic/AINodeBaseClasses.h"
#include "legacyMode/ai/lAINodeLogic.h"
#include "tacticalMode/ai/tAINodeLogic.h"
#include "ai/AILogic/AITree.h"

#include "ai/AIManager.h"

namespace AI
{

static Urho3D::Vector<Urho3D::String> xmlBehaviors =
{
	"AI/AIDefenderPlatform.xml",
	"AI/AIFire.xml",
	"AI/AIPlayer.xml",
	"AI/AIResearchProbe.xml",
	"AI/AIRocket.xml",
	"AI/AIMob.xml",
	"AI/AIPlanet.xml",
	"AI/AIBaseShip.xml"
};

AIManager::AIManager(Context* context)
	: Object(context)
{
	CreateAiNodesPrototypes();
}

std::unique_ptr<AINode> AIManager::GetNodeClone(const Urho3D::String & nodeClassName) const
{
	if (aiNodeStorage_)
		return aiNodeStorage_->GetNodeClone(nodeClassName);

	return nullptr;
}

std::unique_ptr<BehaviorTree> AIManager::GetBehavior(const Urho3D::String& behaviorFileName) const
{
	auto behaviorIt = behaviorAiMap_.find(behaviorFileName);
	if (behaviorIt == behaviorAiMap_.end() || behaviorIt->second == nullptr)
		return nullptr;

	return behaviorIt->second->Clone();
}

void AIManager::LoadBehaviors()
{
	for (auto behaviorFileName : xmlBehaviors)
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		XMLFile* xmlFile = cache->GetResource<XMLFile>(behaviorFileName);
		behaviorAiMap_[behaviorFileName] = std::make_unique<BehaviorTree>(context_, xmlFile);
		URHO3D_LOGINFOF("Loaded behavior: %s", behaviorFileName.CString());
	}
}

void AIManager::CreateAiNodesPrototypes()
{
	aiNodeStorage_ = std::make_unique<AINodeStorage>();

	// Common behavior trees logic, TODO: should be moved to some global AI manager
	aiNodeStorage_->RegisterNodeType("Selector",                           std::make_unique<AI::Nodes::Selector>());
	aiNodeStorage_->RegisterNodeType("Sequence",                           std::make_unique<AI::Nodes::Sequence>());
	aiNodeStorage_->RegisterNodeType("RndSelector",                        std::make_unique<AI::Nodes::RndSelector>());
	aiNodeStorage_->RegisterNodeType("DynSelector",                        std::make_unique<AI::Nodes::DynSelector>());
	aiNodeStorage_->RegisterNodeType("Interruptor",                        std::make_unique<AI::Nodes::Interruptor>());
	aiNodeStorage_->RegisterNodeType("Inverter",                           std::make_unique<AI::Nodes::Inverter>());
	aiNodeStorage_->RegisterNodeType("Successor",                          std::make_unique<AI::Nodes::Successor>());
	aiNodeStorage_->RegisterNodeType("Negator",                            std::make_unique<AI::Nodes::Negator>());

	// legacy mode
	aiNodeStorage_->RegisterNodeType("FollowZeroPlayer",                   std::make_unique<AI::Nodes::FollowZeroPlayer>());
	aiNodeStorage_->RegisterNodeType("FollowOwner",                        std::make_unique<AI::Nodes::FollowOwner>());
	aiNodeStorage_->RegisterNodeType("FollowTarget",                       std::make_unique<AI::Nodes::FollowTarget>());
	aiNodeStorage_->RegisterNodeType("FlyToClosestSolidPlanet",            std::make_unique<AI::Nodes::FlyToClosestSolidPlanet>());
	aiNodeStorage_->RegisterNodeType("FlyToTargetPlanet",                  std::make_unique<AI::Nodes::FlyToTargetPlanet>());
	aiNodeStorage_->RegisterNodeType("FlyToCrystal",                       std::make_unique<AI::Nodes::FlyToCrystal>());
	aiNodeStorage_->RegisterNodeType("SpiralSearch",                       std::make_unique<AI::Nodes::SpiralSearch>());
	aiNodeStorage_->RegisterNodeType("HasCrystalDetector",                 std::make_unique<AI::Nodes::HasCrystalDetector>());

	aiNodeStorage_->RegisterNodeType("HasResourceOnEFactoryAndCDetector", std::make_unique<AI::Nodes::HasResourceOnEFactoryAndCDetector>());
	aiNodeStorage_->RegisterNodeType("HasResourceOnRadar",                std::make_unique<AI::Nodes::HasResourceOnRadar>());
	aiNodeStorage_->RegisterNodeType("HasEveryPanetImprovedRadar",        std::make_unique<AI::Nodes::HasEveryPanetImprovedRadar>());
	aiNodeStorage_->RegisterNodeType("BetterRadarToGetEnergy",            std::make_unique<AI::Nodes::BetterRadarToGetEnergy>());
	aiNodeStorage_->RegisterNodeType("PlanetWithoutRadar",                std::make_unique<AI::Nodes::PlanetWithoutRadar>());
	aiNodeStorage_->RegisterNodeType("PlanetWithoutDefenderPlatform",     std::make_unique<AI::Nodes::PlanetWithoutDefenderPlatform>());
	aiNodeStorage_->RegisterNodeType("BuildEquipmentFactory",             std::make_unique<AI::Nodes::BuildEquipmentFactory>());
	aiNodeStorage_->RegisterNodeType("CreateCrystalDetector",             std::make_unique<AI::Nodes::CreateCrystalDetector>());

	aiNodeStorage_->RegisterNodeType("BuildRadar",                        std::make_unique<AI::Nodes::BuildRadar>());
	aiNodeStorage_->RegisterNodeType("TakeRadarEnergy",                   std::make_unique<AI::Nodes::TakeRadarEnergy>());
	aiNodeStorage_->RegisterNodeType("ImproveRadar",                      std::make_unique<AI::Nodes::ImproveRadar>());
	aiNodeStorage_->RegisterNodeType("BuildDefenderPlatform",             std::make_unique<AI::Nodes::BuildDefenderPlatform>());
	aiNodeStorage_->RegisterNodeType("UseCrystalDetector",                std::make_unique<AI::Nodes::UseCrystalDetector>());
	aiNodeStorage_->RegisterNodeType("FireIfEnemy",                       std::make_unique<AI::Nodes::FireIfEnemy>());

	// tactical mode
	aiNodeStorage_->RegisterNodeType("Fire",                              std::make_unique<AI::Nodes::Fire>());
	aiNodeStorage_->RegisterNodeType("FlyToEnemyBase",                    std::make_unique<AI::Nodes::FlyToEnemyBase>());
	aiNodeStorage_->RegisterNodeType("Stop",                              std::make_unique<AI::Nodes::Stop>());
	aiNodeStorage_->RegisterNodeType("IsEnemyAround",                     std::make_unique<AI::Nodes::IsEnemyAround>());
}
};
