#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>

#include "tacticalMode/equipment/tEquipment.h"
#include "utility/simpleTypes.h"
#include "tacticalMode/equipment/tEquipment.h"

using namespace Urho3D;

class Weapon;

class TEquipmentManager : public LogicComponent
{
	URHO3D_OBJECT(TEquipmentManager, LogicComponent);

public:
	enum Module
	{
		M_Engine,
		M_FieldGenerator,
		M_FirstWeapon,
		M_SecondWeapon,
		M_ThirdWeapon,
		M_UltimateWeapon,
		M_Cargo,
		M_Count
	};

	std::map<Module, TEquipment::EquipmentType> moduleToEquipment_ =
	{
		{ Module::M_Engine, TEquipment::EquipmentType::ET_Engine },
		{ Module::M_FieldGenerator, TEquipment::EquipmentType::ET_FieldGenerator },
		{ Module::M_FirstWeapon, TEquipment::EquipmentType::ET_Weapon },
		{ Module::M_SecondWeapon, TEquipment::EquipmentType::ET_Weapon },
		{ Module::M_ThirdWeapon, TEquipment::EquipmentType::ET_Weapon },
		{ Module::M_ThirdWeapon, TEquipment::EquipmentType::ET_Weapon }
	};

	static void RegisterObject(Context* context);

	TEquipmentManager(Context* context);
	~TEquipmentManager();

	void Create() const;

	Node* GetModulesNode() const;
	Node* GetModuleNode(Module module) const;
	Node* GetOrCreateModuleNode(Module module) const;

	Node* GetElementNode(Module module, const String& elementName);
	Vector<SharedPtr<Node>> GetElementNodes(Module module);
	Vector<Node*> GetElementNodes(TEquipment::EquipmentType type);

	template <typename T>
	T* GetElement(Module module);

	template <typename T>
	Node* GetElementNode(Module module);

	template <typename T>
	PODVector<Node*> GetElementNodesWithComponent(Module module);

	template <typename T>
	Vector<T*> GetElements(Module module);

	Module GetModuleByEquipment(TEquipment::EquipmentType type, Node* elemNode = nullptr);

	TEquipment* GetEquipment(Module module) const;
	bool IsEquipmentUsed(Module module);

	void SetFreeToUse(Module module) const;
	void SetFreeToUse(Node* elementNode) const;

	void AddElementToModule(Module module, Node* elemNode, bool isFreeToUse = false) const;
	void AddElementToModule(Module module, const String& elementName, bool isFreeToUse = false) const;

	void RemoveElementFromModule(Module module, U32 elementID) const;
};

template<typename T>
inline T * TEquipmentManager::GetElement(Module module)
{
	Node* moduleNode = GetModuleNode(module);
	if (moduleNode)
		return moduleNode->GetComponent<T>(true);

	return nullptr;
}

template<typename T>
inline Node * TEquipmentManager::GetElementNode(Module module)
{
	T* component = GetElement<T>(module);
	if (component)
		return component->GetNode();

	return nullptr;
}

template<typename T>
inline PODVector<Node*> TEquipmentManager::GetElementNodesWithComponent(Module module)
{
	PODVector<Node*> result;

	Node* moduleNode = GetModuleNode(module);
	if (!moduleNode)
		return result;

	moduleNode->GetChildrenWithComponent<T>(result, true);
	return result;
}

template<typename T>
inline Vector<T*> TEquipmentManager::GetElements(Module module)
{
	Vector<T*> elements;

	Vector<SharedPtr<Node>> elementNodes = GetElementNodesWithComponent<T>(module);
	for (auto elementNode : elementNodes)
		elements.Push(elementNode->GetComponent<T>(true));

	return elements;
}
