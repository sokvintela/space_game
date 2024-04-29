#include <Urho3D/UI/UIElement.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Core/Context.h>

#include "legacyMode/galaxy/galaxyState.h"

using namespace Urho3D;

GalaxyState::GalaxyState(Urho3D::Context* context)
	: IGameState(context)
{
}

void GalaxyState::Create()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	XMLFile* layout = cache->GetResource<XMLFile>("UI/galaxyState.xml");
	if (!layout)
		return;

	uiStateRoot_->LoadXML(layout->GetRoot(), style);
}

void GalaxyState::Enter()
{
	uiStateRoot_->SetVisible(true);
	uiStateRoot_->UpdateLayout();
}

void GalaxyState::Exit()
{
	uiStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void GalaxyState::Pause()
{
}

void GalaxyState::Resume()
{
}
