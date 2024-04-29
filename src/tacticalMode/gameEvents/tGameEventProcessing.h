#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

class TGameEventProcessing : public Object
{
	URHO3D_OBJECT(TGameEventProcessing, Object);

public:
	TGameEventProcessing(Urho3D::Context * context);
	~TGameEventProcessing();

	virtual void SubscribeToNetworkEvents();

	void SetScene(Scene* scene);
	bool IsServerConnection() const;

	void HandleEquipmentMove(StringHash eventType, VariantMap& eventData);
	void HandleChangeEquipment(StringHash eventType, VariantMap& eventData);

private:
	WeakPtr<Scene> scene_;
};
