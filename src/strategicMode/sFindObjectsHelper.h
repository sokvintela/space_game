#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Scene/Node.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class SFindObjectsHelper : public Object
{
	URHO3D_OBJECT(SFindObjectsHelper, Object);

public:
	SFindObjectsHelper(Context* context, Node* controlNode, UIElement* uiSpaceState);
	~SFindObjectsHelper();

	void FindClosestObjects(UIElement* objectWindow, Vector2 mousePos);

	template<typename T>
	void SetOffShipFromObject(F32 operabilityRadius, Vector2 mousePos, F32 mouseHoveringArea);

	template<typename T>
	void FindClosestObject(Node* systemNode, Vector2 mousePos, F32 visibilityRadius, F32 operabilityRadius, F32 mouseHoveringArea);

	Node* GetEnteredNode() const;
	Node* GetHoveringNode() const;

	void SetEnteredNode(Node* enteredNode);

private:
	Node* controlNode_;

	WeakPtr<Node> enteredNode_;
	WeakPtr<Node> hoveringNode_;

	WeakPtr<UIElement> objectWindow_;
	WeakPtr<Text> typeText_;
};

template<typename T>
inline void SFindObjectsHelper::SetOffShipFromObject(F32 operabilityRadius, Vector2 mousePos, F32 mouseHoveringArea)
{
	if (!controlNode_)
		return;

	if (enteredNode_)
	{
		F32 distanceToObject = (enteredNode_->GetWorldPosition2D() - controlNode_->GetWorldPosition2D()).Length();
		if (enteredNode_->HasComponent<T>())
		{
			if (distanceToObject > operabilityRadius)
				enteredNode_ = nullptr;
		}
	}

	if (hoveringNode_)
	{
		F32 distanceToObject = (mousePos - controlNode_->GetWorldPosition2D()).Length();
		if (hoveringNode_->HasComponent<T>())
		{
			if (distanceToObject > mouseHoveringArea)
			{
				hoveringNode_ = nullptr;
				objectWindow_->SetVisible(false);
			}
		}
	}
}

template<typename T>
inline void SFindObjectsHelper::FindClosestObject(Node* systemNode, Vector2 mousePos, F32 visibilityRadius, F32 operabilityRadius, F32 mouseHoveringArea)
{
	if (!controlNode_ || !systemNode)
		return;

	static const F32 starSystemsToCheckRadius = 400.0f;
	if ((systemNode->GetWorldPosition2D() - controlNode_->GetWorldPosition2D()).Length() < starSystemsToCheckRadius)
	{
		PODVector<Node*> objectNodes;
		systemNode->GetChildrenWithComponent<T>(objectNodes, true);

		for (auto objectNode : objectNodes)
		{
			F32 distanceToObject = (objectNode->GetWorldPosition2D() - controlNode_->GetWorldPosition2D()).Length();
			if (distanceToObject < visibilityRadius)
			{
				if (distanceToObject < operabilityRadius)
				{
					enteredNode_ = objectNode;
				}

				if ((objectNode->GetWorldPosition2D() - mousePos).Length() < mouseHoveringArea)
				{
					hoveringNode_ = objectNode;
				}
			}
		}
	}
}
