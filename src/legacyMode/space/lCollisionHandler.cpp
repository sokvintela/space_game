#include <Urho3D/Urho2D/PhysicsEvents2D.h>
#include <Urho3D/Scene/Node.h>

#include "tacticalMode/equipment/weapon/weapon.h"
#include "legacyMode/galaxy/spaceObjects/crystal.h"
#include "legacyMode/managers/lResourceManager.h"
#include "legacyMode/galaxy/spaceObjects/lDebris.h"
#include "legacyMode/spaceship/lDurability.h"

#include "legacyMode/space/lCollisionHandler.h"

LCollisionHandler::LCollisionHandler(Context * context)
	: Object(context)
{
	SubscribeToEvent(E_PHYSICSBEGINCONTACT2D, URHO3D_HANDLER(LCollisionHandler, HandleCollision));
}

LCollisionHandler::~LCollisionHandler()
{
	UnsubscribeFromEvent(E_PHYSICSBEGINCONTACT2D);
}

void LCollisionHandler::HandleCollision(StringHash eventType, VariantMap & eventData)
{
	Node* nodeA = static_cast<Node*>(eventData[PhysicsBeginContact2D::P_NODEA].GetPtr());
	Node* nodeB = static_cast<Node*>(eventData[PhysicsBeginContact2D::P_NODEB].GetPtr());

	if (nodeA == nodeB)
		return;

	Node* weaponNodeA = nodeA->GetParent();
	Node* weaponNodeB = nodeB->GetParent();

	Weapon* weaponA = nullptr;
	Weapon* weaponB = nullptr;

	if (weaponNodeA)
		weaponA = weaponNodeA->GetComponent<Weapon>();

	if (weaponNodeB)
		weaponB = weaponNodeB->GetComponent<Weapon>();

	LDurability* durabilityA = nodeA->GetComponent<LDurability>();
	LDurability* durabilityB = nodeB->GetComponent<LDurability>();

	Crystal* crystalA = nodeA->GetComponent<Crystal>();
	Crystal* crystalB = nodeB->GetComponent<Crystal>();

	LDebris* debrisA = nodeA->GetComponent<LDebris>();
	LDebris* debrisB = nodeB->GetComponent<LDebris>();

	if (durabilityB && weaponA)
	{
		durabilityA = durabilityB;
		weaponB = weaponA;
	}

	if (durabilityB && debrisA)
	{
		durabilityA = durabilityB;
		debrisB = debrisA;
	}

	if (durabilityB && crystalA)
	{
		durabilityA = durabilityB;
		crystalB = crystalA;
	}

	// weapon
	if (durabilityA && weaponB)
	{
		durabilityA->TakeDamage(weaponB->GetDamage());
		nodeB->Remove();
	}

	// debris
	if (durabilityA && debrisB)
	{
		LResourceManager* objectRManager = nodeA->GetComponent<LResourceManager>();
		LResourceManager* debrisRManager = nodeB->GetComponent<LResourceManager>();
		if (objectRManager && debrisRManager)
			objectRManager->ChangeResourceValue(LGameResource::ResourceType::Metals, debrisRManager->GetResourceValue(LGameResource::ResourceType::Metals));

		nodeB->Remove();
	}

	// crystal
	if (durabilityA && crystalB)
	{
		F32 adition = 0.f;

		LGameResource* energyB = nodeB->GetComponent<LGameResource>(true);
		if (energyB)
			adition = energyB->GetResourceValue();

		LResourceManager* managerA = nodeA->GetComponent<LResourceManager>();
		if (managerA)
			managerA->ChangeResourceValue(LGameResource::ResourceType::Energy, adition);

		nodeB->Remove();
	}

	// module + module
	// TODO: disabled collision damage
	/*
	if (moduleA && moduleB)
	{
	moduleA->ReceiveDamage();
	moduleB->ReceiveDamage();
	}
	*/
}
