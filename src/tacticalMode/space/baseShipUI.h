#pragma once

#include <Urho3D/Core/Object.h>
#include "utility/simpleTypes.h"

using namespace Urho3D;

namespace Urho3D
{
	class Node;
	class UIElement;
	class ListView;
};

class BaseShipUI : public Object
{
	URHO3D_OBJECT(BaseShipUI, Object);

public:
	BaseShipUI(Context* context, Node* controlNode, UIElement* uiSpaceState);
	~BaseShipUI();

	void HandleUpdateUI();
	void SwitchItemUI() const;
	void SetVisibility(bool visibility);

private:
	Node* controlNode_;

	Vector<ListView*> equipmentList_;
	HashMap<Button*, Node*> equipmentMapping_;

	void HandleCreateEquipment(StringHash eventType, VariantMap& eventData);
};
