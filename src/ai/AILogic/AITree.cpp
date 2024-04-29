#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Core/Context.h>

#include "ai/AIManager.h"
#include "ai/AILogic/AINode.h"

#include "ai/AILogic/AITree.h"

using namespace Urho3D;

namespace AI
{
void BehaviorTree::LoadBehaviorFromXML(Context* context, XMLFile* xmlFile)
{
	if (!xmlFile)
		return;

	XMLElement root = xmlFile->GetRoot("node");

	if (root != XMLElement::EMPTY)
		aiRoot_ = BehaviorTree::CreateNode(context, root);
		
	runningDynamicNode_ = nullptr;
}

AINode::NodeUniquePtr BehaviorTree::CreateNode(Context* context, const XMLElement& xmlNode)
{
	String nodeClass = xmlNode.GetAttribute("class");

	AIManager* aiManager = context->GetSubsystem<AIManager>();
	if (!aiManager)
		return nullptr;

	auto node = aiManager->GetNodeClone(nodeClass);
	if (!node)
		return nullptr;

	bool initResult = node->initFromXML(xmlNode);
	if (!initResult)
		return nullptr;

	for (auto childXmlNode = xmlNode.GetChild(); childXmlNode != XMLElement::EMPTY; childXmlNode = childXmlNode.GetNext())
	{
		AINode::NodeUniquePtr child = CreateNode(context, childXmlNode);
		if (child != nullptr)
			node->AddChild(std::move(child));
	}

	return node;
}

BehaviorTree::BehaviorTree()
	: runningDynamicNode_(nullptr)
{
}

BehaviorTree::BehaviorTree(Context* context, XMLFile* xmlFile)
	: runningDynamicNode_(nullptr)
{
	LoadBehaviorFromXML(context, xmlFile);
}

BehaviorTree::~BehaviorTree()
{
	aiRoot_.reset();
	runningDynamicNode_ = nullptr;
}

std::unique_ptr<BehaviorTree> BehaviorTree::Clone() const
{
	std::unique_ptr<BehaviorTree> copyTree = std::make_unique<BehaviorTree>();
	if (aiRoot_)
	{
		copyTree->aiRoot_ = aiRoot_->Clone();
		copyTree->runningDynamicNode_ = nullptr;
	}
	return copyTree;
}

void BehaviorTree::Process(F32 timeStep)
{
	if (aiRoot_)
	{
		if (runningDynamicNode_)
		{
			NodeReturnStatus result = runningDynamicNode_->Process(timeStep);
			if (result != NodeReturnStatus::Running)
				runningDynamicNode_ = nullptr;
		}
		else
			/* NodeReturnStatus result = */ aiRoot_->Process(timeStep);
	}
}

void BehaviorTree::SetRunningDynamicNode(AINode* node)
{
	runningDynamicNode_ = node;
}

void BehaviorTree::SetAiElement(Node* aiElement)
{
	if(aiRoot_)
		aiRoot_->SetAiElement(aiElement);
}
};
