#pragma once

#include <Urho3D/UI/FileSelector.h>

#include "stateManager/gameStates.h"

class LoadingGameState : public IGameState
{
	URHO3D_OBJECT(LoadingGameState, IGameState);
public:
	LoadingGameState(Urho3D::Context * context);
	virtual ~LoadingGameState() = default;

	virtual void Create();
	virtual void Enter();
	virtual void Exit();
	virtual void Pause();
	virtual void Resume();

private:

	SharedPtr<FileSelector> fileSelector_;

	void HandleBackClick(StringHash eventType, VariantMap& eventData);
	void HandleLoadClick(StringHash eventType, VariantMap& eventData);
};
