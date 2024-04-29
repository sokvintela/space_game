#pragma once

#include <Urho3D/Engine/Application.h>

#include "utility/simpleTypes.h"

static const U32 NUM_RACE = 5;
static const U32 NUM_SOURCE_TYPES = NUM_RACE;

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

class SpaceGame : public Application
{
	URHO3D_OBJECT(SpaceGame, Application);

public:
	/// Construct.
	SpaceGame(Context* context);

	/// Setup before engine initialization. Modifies the engine parameters.
	virtual void Setup();
	/// Setup after engine initialization. Creates the logo, console & debug HUD.
	virtual void Start();
	/// Cleanup after the main loop. Called by Application.
	virtual void Stop();

private:

	String GenerateWindowTitle() const;

	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	void HandleGameExit(StringHash eventType, VariantMap& eventData);

	// optionally launch script on start
	bool scriptedRun_;
	String scriptFileName_;
};
