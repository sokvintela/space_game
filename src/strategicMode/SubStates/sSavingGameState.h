#pragma once

#include "stateManager/gameStates.h"

namespace Urho3D
{
	class FileSelector;
}

class SSavingGameState : public ISubState
{
	URHO3D_OBJECT(SSavingGameState, ISubState);
public:
	SSavingGameState(Urho3D::Context * context, UIElement * uiSubState);
	virtual ~SSavingGameState() = default;

	virtual void Create(Node* targetNode = nullptr);
	virtual void Enter(Node* targetNode = nullptr);
	virtual void Exit();

private:

	SharedPtr<FileSelector> fileSelector_;

	void HandleBackClick(StringHash eventType, VariantMap& eventData);
	void HandleSaveClick(StringHash eventType, VariantMap& eventData);
};
