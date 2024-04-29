#pragma once

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Component.h>

namespace Urho3D
{
	class UIElement;
}

using namespace Urho3D;

class IGameState : public Object
{
	URHO3D_OBJECT(IGameState, Object);
public:
	IGameState(Context * context);
	virtual ~IGameState() = default;
	/// create
	virtual void Create() = 0;
	/// enter 
	virtual void Enter() = 0;
	/// exit 
	virtual void Exit() = 0;
	/// pause
	virtual void Pause() = 0;
	/// resume
	virtual void Resume() = 0;

	virtual void EnterSubstate(int) { }
	virtual void ExitSubstate() { }

	Scene* GetScene() const { return scene_; }
protected:

	WeakPtr<UIElement>	uiStateRoot_;
	Scene*				scene_;
};

class ISubState : public Object
{
	URHO3D_OBJECT(ISubState, Object);
public:
	ISubState(Context * context, UIElement* uiSubState);
	virtual ~ISubState() = default;
	/// create
	virtual void Create(Node* targetNode = nullptr) = 0;
	/// enter 
	virtual void Enter(Node* targetNode = nullptr) = 0;
	/// exit 
	virtual void Exit() = 0;

	virtual void SubscribeToNetworkEvents() { };

	WeakPtr<IGameState> parentState_;
protected:

	WeakPtr<UIElement> uiSubStateRoot_;
};
