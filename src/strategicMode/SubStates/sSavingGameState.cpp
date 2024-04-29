#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/UI/FileSelector.h>
#include <Urho3D/UI/Sprite.h>

#include "utility/sharedData.h"

#include "strategicMode/SubStates/sSavingGameState.h"

using namespace Urho3D;

SSavingGameState::SSavingGameState(Urho3D::Context * context, UIElement* uiSubState) :
	ISubState(context, uiSubState)
{
}

void SSavingGameState::Create(Node* targetNode)
{
	// Create a new sprite, set it to use the texture
	Texture2D* planetModeSprite = GetSubsystem<ResourceCache>()->GetResource<Texture2D>("Urho2D/backgrounds/planet_back.jpg");
	Sprite* uiSprite = new Sprite(context_);
	uiSprite->SetTexture(planetModeSprite);
	uiSprite->SetSize(uiSubStateRoot_->GetSize());
	uiSubStateRoot_->AddChild(uiSprite);
}

void SSavingGameState::Enter(Node* targetNode)
{
	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	String programDir = fileSystem->GetProgramDir();

	fileSelector_ = new FileSelector(context_);

	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
	fileSelector_->SetDefaultStyle(style);
	fileSelector_->SetPath(programDir + "Saves");
	fileSelector_->SetButtonTexts("save", "back");

	uiSubStateRoot_->SetDefaultStyle(style);
	uiSubStateRoot_->SetAlignment(HA_CENTER, VA_CENTER);

	Window* fileWindow = fileSelector_->GetWindow();
	fileWindow->SetAlignment(HA_CENTER, VA_CENTER);
	uiSubStateRoot_->AddChild(fileWindow);

	SubscribeToEvent(fileSelector_->GetOKButton(), E_PRESSED, URHO3D_HANDLER(SSavingGameState, HandleSaveClick));
	SubscribeToEvent(fileSelector_->GetCancelButton(), E_PRESSED, URHO3D_HANDLER(SSavingGameState, HandleBackClick));
}

void SSavingGameState::Exit()
{
	fileSelector_ = nullptr;
	uiSubStateRoot_->SetVisible(false);

	UnsubscribeFromAllEvents();
}

void SSavingGameState::HandleBackClick(StringHash eventType, VariantMap & eventData)
{
	Exit();
	parentState_->ExitSubstate();
}

void SSavingGameState::HandleSaveClick(StringHash eventType, VariantMap & eventData)
{
	SharedData* sharedData = GetSubsystem<SharedData>();
	sharedData->saveFileName_ = fileSelector_->GetPath() + fileSelector_->GetFileName();

	Exit();
	parentState_->ExitSubstate();
}
