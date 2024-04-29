#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>

#include "legacyMode/equipment/lEquipment.h"
#include "utility/simpleTypes.h"
#include "legacyMode/equipment/modules.h"

using namespace Urho3D;

class Weapon;

class LEquipmentManager : public LogicComponent
{
	URHO3D_OBJECT(LEquipmentManager, LogicComponent);

public:
	static void RegisterObject(Context* context);

	LEquipmentManager(Context* context);
	~LEquipmentManager();

	void Create() const;

	Node* GetOrCreateCargoNode() const;
	Node* GetCargoNode() const;
	Node* GetModulesNode() const;
	Node* GetModuleNode(Modules::Module moduleType) const;
	Node* GetOrCreateModuleNode(Modules::Module moduleType) const;

	Vector<SharedPtr<Node>> GetCargoElements() const;
	Vector<Weapon*> GetWeapon() const;
	Vector<SharedPtr<Node>> GetEquipmentElements() const;

	template<typename T>
	T* GetEquipmentItem() const;

	template<typename T>
	Node* GetEquipmentItemNode() const;

	void SetFreeToUse(Modules::Module moduleType, LEquipment::EquipmentType elementType) const;

	void AddElementToModule(Modules::Module moduleType, Node* elemNode) const;
	void AddElementToModule(Modules::Module moduleType, LEquipment::EquipmentType elementType, const String& elementName) const;

	void AddElementToCargo(Node* elemNode) const;

	void RemoveElementFromModule(Modules::Module moduleType, U32 elementID) const;
	void RemoveElementFromCargo(U32 elementID) const;

	Weapon* GetWeapon(Modules::Module side) const;
	Node* GetWeaponNode(Modules::Module side) const;

	LEquipment* GetEquipment(Modules::Module moduleType) const;
};

template<typename T>
inline T* LEquipmentManager::GetEquipmentItem() const
{
	Node* modulesNode = GetModulesNode();

	if (modulesNode)
	{
		return modulesNode->GetComponent<T>(true);
	}

	return nullptr;
}

template<typename T>
inline Node* LEquipmentManager::GetEquipmentItemNode() const
{
	T* component = GetEquipmentItem<T>();
	if (component)
		return component->GetNode();

	return nullptr;
}
