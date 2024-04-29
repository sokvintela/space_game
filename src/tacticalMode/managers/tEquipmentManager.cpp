#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>

#include "tacticalMode/equipment/weapon/weapon.h"
#include "prefabs/prefabsStorage.h"
#include "equipment/shipEngine.h"
#include "equipment/energyFieldGenerator.h"

#include "tacticalMode/managers/tEquipmentManager.h"

String modules = "modules";
String engine = "engine";
String fieldGenerator = "fieldGenerator";
String firstWeapon = "firstWeapon";
String secondWeapon = "secondWeapon";
String thirdWeapon = "thirdWeapon";
String ultimateWeapon = "ultimateWeapon";
String cargo = "cargo";

void TEquipmentManager::RegisterObject(Context* context)
{
	context->RegisterFactory<TEquipmentManager>();
}

TEquipmentManager::TEquipmentManager(Context * context) :
	LogicComponent(context)
{
}

TEquipmentManager::~TEquipmentManager()
{
}

void TEquipmentManager::Create() const
{
	Node* modulesNode = node_->CreateChild(modules);
}

Node* TEquipmentManager::GetModulesNode() const
{
	return node_->GetChild(modules);
}

Node* TEquipmentManager::GetModuleNode(Module module) const
{
	Node* modulesNode = GetModulesNode();
	if (!modulesNode)
		return nullptr;

	Node* moduleNode = nullptr;
	switch (module)
	{
		case Module::M_Engine:
		{
			moduleNode = modulesNode->GetChild(engine);
			break;
		}
		case Module::M_FieldGenerator:
		{
			moduleNode = modulesNode->GetChild(fieldGenerator);
			break;
		}
		case Module::M_FirstWeapon:
		{
			moduleNode = modulesNode->GetChild(firstWeapon);
			break;
		}
		case Module::M_SecondWeapon:
		{
			moduleNode = modulesNode->GetChild(secondWeapon);
			break;
		}
		case Module::M_ThirdWeapon:
		{
			moduleNode = modulesNode->GetChild(thirdWeapon);
			break;
		}
		case Module::M_UltimateWeapon:
		{
			moduleNode = modulesNode->GetChild(ultimateWeapon);
			break;
		}
		case Module::M_Cargo:
		{
			moduleNode = modulesNode->GetChild(cargo);
			break;
		}
	}

	return moduleNode;
}

Node* TEquipmentManager::GetOrCreateModuleNode(Module module) const
{
	Node* modulesNode = GetModulesNode();
	if (!modulesNode)
		return nullptr;

	Node* moduleNode = GetModuleNode(module);
	if (!moduleNode)
		switch (module)
		{
			case Module::M_Engine:
			{
				moduleNode = modulesNode->CreateChild(engine);
				break;
			}
			case Module::M_FieldGenerator:
			{
				moduleNode = modulesNode->CreateChild(fieldGenerator);
				break;
			}
			case Module::M_FirstWeapon:
			{
				moduleNode = modulesNode->CreateChild(firstWeapon);
				break;
			}
			case Module::M_SecondWeapon:
			{
				moduleNode = modulesNode->CreateChild(secondWeapon);
				break;
			}
			case Module::M_ThirdWeapon:
			{
				moduleNode = modulesNode->CreateChild(thirdWeapon);
				break;
			}
			case Module::M_UltimateWeapon:
			{
				moduleNode = modulesNode->CreateChild(ultimateWeapon);
				break;
			}
			case Module::M_Cargo:
			{
				moduleNode = modulesNode->CreateChild(cargo);
				break;
			}
		}

	return moduleNode;
}
Node * TEquipmentManager::GetElementNode(Module module, const String & elementName)
{
	Node* moduleNode = GetModuleNode(module);
	if (moduleNode)
		return moduleNode->GetChild(elementName);

	return nullptr;
}
Vector<SharedPtr<Node>> TEquipmentManager::GetElementNodes(Module module)
{
	Node* moduleNode = GetModuleNode(module);
	if (moduleNode)
		return moduleNode->GetChildren();

	return Vector<SharedPtr<Node>>();
}
Vector<Node*> TEquipmentManager::GetElementNodes(TEquipment::EquipmentType type)
{
	Vector<Node*> elementNodes;

	for (auto& moduleToEquipment : moduleToEquipment_)
	{
		if (moduleToEquipment.second == type)
		{
			Node* module = GetModuleNode(moduleToEquipment.first);
			if (!module)
				continue;

			for(auto& child : module->GetChildren())
				elementNodes.Push(child);
		}
	}

	return elementNodes;
}

TEquipmentManager::Module TEquipmentManager::GetModuleByEquipment(TEquipment::EquipmentType type, Node* elemNode)
{
	for (auto mappingItem : moduleToEquipment_)
	{
		if (mappingItem.second == type)
		{
			if (elemNode)
			{
				Vector<SharedPtr<Node>> elementNodes = GetElementNodes(mappingItem.first);
				for (Node* elementNode : elementNodes)
					if (elementNode == elemNode)
						return mappingItem.first;
			}
			else
				return mappingItem.first;
		}
	}

	return Module::M_Count;
}

TEquipment * TEquipmentManager::GetEquipment(Module module) const
{
	Node* moduleNode = GetModuleNode(module);
	if (moduleNode)
		return moduleNode->GetComponent<TEquipment>(true);

	return nullptr;
}

bool TEquipmentManager::IsEquipmentUsed(Module module)
{
	TEquipment* equipment = GetEquipment(module);
	if (equipment && equipment->Use())
		return true;

	return false;
}

void TEquipmentManager::SetFreeToUse(Module module) const
{
	TEquipment* equipment = GetEquipment(module);
	if (equipment)
		equipment->SetFreeToUse();
}

void TEquipmentManager::SetFreeToUse(Node * elementNode) const
{
	TEquipment* equipment = elementNode->GetComponent<TEquipment>();
	if (equipment)
		equipment->SetFreeToUse();
}

void TEquipmentManager::AddElementToModule(Module module, Node * elemNode, bool isFreeToUse) const
{
	Node* moduleNode = GetOrCreateModuleNode(module);
	if (!moduleNode)
		return;

	moduleNode->RemoveAllChildren();
	moduleNode->AddChild(elemNode);

	if (isFreeToUse)
		SetFreeToUse(elemNode);
}

void TEquipmentManager::AddElementToModule(Module module, const String & elementName, bool isFreeToUse) const
{
	Node* moduleNode = GetOrCreateModuleNode(module);
	if (!moduleNode)
		return;

	auto moduleIt = moduleToEquipment_.find(module);
	if (moduleIt == moduleToEquipment_.end())
		return;

	Node* equipmentNode = GetSubsystem<PrefabsStorage>()->CreateEquipment(node_, moduleIt->second, elementName);
	if (!equipmentNode)
		return;

	moduleNode->RemoveAllChildren();
	moduleNode->AddChild(equipmentNode);

	if (isFreeToUse)
		SetFreeToUse(equipmentNode);
}

void TEquipmentManager::RemoveElementFromModule(Module module, U32 elementID) const
{
	Node* moduleNode = GetModuleNode(module);
	if (!moduleNode)
		return;

	Node* elementNode = moduleNode->GetChild(elementID);
	if (elementNode)
		elementNode->Remove();
}
