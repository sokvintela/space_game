#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Scene.h>

#include "tacticalMode/equipment/weapon/weapon.h"
#include "prefabs/prefabsStorage.h"
#include "equipment/shipEngine.h"
#include "legacyMode/equipment/hyperDrive.h"
#include "equipment/energyFieldGenerator.h"
#include "legacyMode/equipment/repairDrone.h"
#include "legacyMode/equipment/crystalDetector.h"

#include "legacyMode/managers/lEquipmentManager.h"

String lcargo = "cargo";
String lmodules = "modules";
String lengine = "engine";
String lhyperEngine = "hyperEngine";
String lfirstWeapon = "firstWeapon";
String lsecondWeapon = "secondWeapon";
String lequipment = "equipment";

void LEquipmentManager::RegisterObject(Context* context)
{
	context->RegisterFactory<LEquipmentManager>();
}

LEquipmentManager::LEquipmentManager(Context * context) :
	LogicComponent(context)
{
}

LEquipmentManager::~LEquipmentManager()
{
}

void LEquipmentManager::Create() const
{
	Node* modulesNode = node_->CreateChild(lmodules);
}

Vector<SharedPtr<Node>> LEquipmentManager::GetCargoElements() const
{
	Node* cargoNode = GetCargoNode();
	if (!cargoNode)
		return Vector<SharedPtr<Node>>();

	return cargoNode->GetChildren();
}

Vector<Weapon*> LEquipmentManager::GetWeapon() const
{
	Vector<Weapon*> result;

	Node* modulesNode = GetModuleNode(Modules::Module::M_FirstWeaponPlace);
	if (!modulesNode)
		return result;

	for (auto moduleNode : modulesNode->GetChildren())
	{
		Weapon* weapon = moduleNode->GetComponent<Weapon>(true);
		if (weapon)
			result.Push(weapon);
	}

	return result;
}

Node* LEquipmentManager::GetOrCreateCargoNode() const
{
	Node* cargoNode = node_->GetChild(lcargo);
	if (!cargoNode)
		cargoNode = node_->CreateChild(lcargo);

	return cargoNode;
}

Node* LEquipmentManager::GetCargoNode() const
{
	return node_->GetChild(lcargo);
}

Node* LEquipmentManager::GetModulesNode() const
{
	return node_->GetChild(lmodules);
}

Node* LEquipmentManager::GetModuleNode(Modules::Module moduleType) const
{
	Node* modulesNode = GetModulesNode();
	if (!modulesNode)
		return nullptr;

	Node* moduleNode = nullptr;
	switch (moduleType)
	{
		case Modules::Module::M_EnginePlace:
		{
			moduleNode = modulesNode->GetChild(lengine);
			break;
		}
		case Modules::Module::M_HyperEnginePlace:
		{
			moduleNode = modulesNode->GetChild(lhyperEngine);
			break;
		}
		case Modules::Module::M_FirstWeaponPlace:
		{
			moduleNode = modulesNode->GetChild(lfirstWeapon);
			break;
		}
		case Modules::Module::M_SecondWeaponPlace:
		{
			moduleNode = modulesNode->GetChild(lsecondWeapon);
			break;
		}
		case Modules::Module::M_OtherEquipmentPlace:
		{
			moduleNode = modulesNode->GetChild(lequipment);
			break;
		}
	}

	return moduleNode;
}

Node* LEquipmentManager::GetOrCreateModuleNode(Modules::Module moduleType) const
{
	Node* modulesNode = GetModulesNode();
	if (!modulesNode)
		return nullptr;

	Node* moduleNode = nullptr;
	switch (moduleType)
	{
		case Modules::Module::M_EnginePlace:
		{
			moduleNode = modulesNode->GetChild(lengine);
			if (!moduleNode)
				moduleNode = modulesNode->CreateChild(lengine);

			break;
		}
		case Modules::Module::M_HyperEnginePlace:
		{
			moduleNode = modulesNode->GetChild(lhyperEngine);
			if (!moduleNode)
				moduleNode = modulesNode->CreateChild(lhyperEngine);

			break;
		}
		case Modules::Module::M_FirstWeaponPlace:
		{
			moduleNode = modulesNode->GetChild(lfirstWeapon);
			if (!moduleNode)
				moduleNode = modulesNode->CreateChild(lfirstWeapon);

			break;
		}
		case Modules::Module::M_SecondWeaponPlace:
		{
			moduleNode = modulesNode->GetChild(lsecondWeapon);
			if (!moduleNode)
				moduleNode = modulesNode->CreateChild(lsecondWeapon);

			break;
		}
		case Modules::Module::M_OtherEquipmentPlace:
		{
			moduleNode = modulesNode->GetChild(lequipment);
			if (!moduleNode)
				moduleNode = modulesNode->CreateChild(lequipment);

			break;
		}
	}

	return moduleNode;
}

Weapon* LEquipmentManager::GetWeapon(Modules::Module side) const
{
	Node* weaponNode = GetModuleNode(side);
	if (!weaponNode)
		return nullptr;

	return weaponNode->GetComponent<Weapon>(true);
}

Node* LEquipmentManager::GetWeaponNode(Modules::Module side) const
{
	Weapon* weapon = GetWeapon(side);
	if (weapon)
		return weapon->GetNode();

	return nullptr;
}

LEquipment * LEquipmentManager::GetEquipment(Modules::Module moduleType) const
{
	Node* moduleNode = GetModuleNode(moduleType);
	if (!moduleNode)
		return nullptr;

	return moduleNode->GetComponent<LEquipment>(true);
}

Vector<SharedPtr<Node>> LEquipmentManager::GetEquipmentElements() const
{
	Node* equipmentNode = GetModuleNode(Modules::Module::M_OtherEquipmentPlace);
	if (!equipmentNode)
		return Vector<SharedPtr<Node>>();

	return equipmentNode->GetChildren();
}

void LEquipmentManager::SetFreeToUse(Modules::Module moduleType, LEquipment::EquipmentType elementType) const
{
	Node* moduleNode = GetModuleNode(moduleType);
	if (!moduleNode)
		return;

	Node* elementNode = nullptr;
	switch (elementType)
	{
	case LEquipment::EquipmentType::ET_Engine:
	{
		ShipEngine* engine = moduleNode->GetComponent<ShipEngine>(true);
		if (engine)
			elementNode = engine->GetNode();

		break;
	}
	case LEquipment::EquipmentType::ET_HyperDrive:
	{
		HyperDrive* hyperEngine = moduleNode->GetComponent<HyperDrive>(true);
		if (hyperEngine)
			elementNode = hyperEngine->GetNode();

		break;
	}
	case LEquipment::EquipmentType::ET_Weapon:
	{
		Weapon* weapon = moduleNode->GetComponent<Weapon>(true);
		if (weapon)
			elementNode = weapon->GetNode();

		break;
	}
	case LEquipment::EquipmentType::ET_FieldGenerator:
	{
		EnergyFieldGenerator* generator = moduleNode->GetComponent<EnergyFieldGenerator>(true);
		if (generator)
			elementNode = generator->GetNode();

		break;
	}
	case LEquipment::EquipmentType::ET_RepairDrone:
	{
		RepairDrone* drone = moduleNode->GetComponent<RepairDrone>(true);
		if (drone)
			elementNode = drone->GetNode();

		break;
	}
	case LEquipment::EquipmentType::ET_CrystalDetector:
	{
		CrystalDetector* detector = moduleNode->GetComponent<CrystalDetector>(true);
		if (detector)
			elementNode = detector->GetNode();

		break;
	}
	}

	LEquipment* equipment = elementNode->GetComponent<LEquipment>();
	if (equipment)
		equipment->SetFreeToUse();
}

void LEquipmentManager::AddElementToModule(Modules::Module moduleType, Node * elemNode) const
{
	Node* moduleNode = GetOrCreateModuleNode(moduleType);
	if (moduleNode)
		moduleNode->AddChild(elemNode);
}

void LEquipmentManager::AddElementToModule(Modules::Module moduleType, LEquipment::EquipmentType elementType, const String & elementName) const
{
	Node* moduleNode = GetOrCreateModuleNode(moduleType);
	if (!moduleNode)
		return;

	Node* equipmentNode = GetSubsystem<PrefabsStorage>()->CreateLegacyEquipment(node_, elementType, elementName);
	if (equipmentNode)
		moduleNode->AddChild(equipmentNode);
}

void LEquipmentManager::AddElementToCargo(Node* elemNode) const
{
	Node* cargoNode = GetOrCreateCargoNode();
	if (cargoNode)
		cargoNode->AddChild(elemNode);
}

void LEquipmentManager::RemoveElementFromModule(Modules::Module moduleType, U32 elementID) const
{
	Node* moduleNode = GetModuleNode(moduleType);
	if (!moduleNode)
		return;

	Node* elementNode = moduleNode->GetChild(elementID);
	if (elementNode)
		elementNode->Remove();
}

void LEquipmentManager::RemoveElementFromCargo(U32 elementID) const
{
	Node* moduleNode = GetCargoNode();
	if (!moduleNode)
		return;

	Node* elementNode = moduleNode->GetChild(elementID);
	if (elementNode)
		elementNode->Remove();
}
