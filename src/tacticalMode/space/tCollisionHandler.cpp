#include <Urho3D/Urho2D/PhysicsEvents2D.h>
#include <Urho3D/Scene/Node.h>

#include "tacticalMode/equipment/weapon/weapon.h"
#include "tacticalMode/managers/tResourceManager.h"
#include "tacticalMode/starSystem/tDebris.h"
#include "tacticalMode/spaceship/tDurability.h"
#include "tacticalMode/managers/tDataManager.h"

#include "tacticalMode/space/tCollisionHandler.h"

TCollisionHandler::TCollisionHandler(Context * context)
	: Object(context)
{
	SubscribeToEvent(E_PHYSICSBEGINCONTACT2D, URHO3D_HANDLER(TCollisionHandler, HandleCollision));
}

TCollisionHandler::~TCollisionHandler()
{
	UnsubscribeFromEvent(E_PHYSICSBEGINCONTACT2D);
}

void TCollisionHandler::HandleCollision(StringHash eventType, VariantMap & eventData)
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

	TDebris* debrisA = nodeA->GetComponent<TDebris>();
	TDebris* debrisB = nodeB->GetComponent<TDebris>();

	TDataManager* objectA = nodeA->GetComponent<TDataManager>();
	TDataManager* objectB = nodeB->GetComponent<TDataManager>();

	// if debris, than to hell data managers
	// debris
	if (objectA && debrisB)
	{
		TResourceManager* objectRManager = nodeA->GetComponent<TResourceManager>();
		TResourceManager* debrisRManager = nodeB->GetComponent<TResourceManager>();
		if (objectRManager && debrisRManager)
			objectRManager->ChangeResourceValue(TGameResource::ResourceType::Metals, debrisRManager->GetResourceValue(TGameResource::ResourceType::Metals));

		nodeB->Remove();
		return;
	}

	if (objectB && weaponA)
	{
		objectA = objectB;
		weaponB = weaponA;
	}

	if (objectB && debrisA)
	{
		objectA = objectB;
		debrisB = debrisA;
	}

	// weapon
	if (objectA && weaponB)
	{
		TDurability* durabilityA = nodeA->GetComponent<TDurability>();
		if (durabilityA)
			durabilityA->TakeDamage(weaponB->GetDamage());

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
