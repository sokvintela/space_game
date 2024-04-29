#include <Urho3D/Scene/Node.h>
#include <Urho3D/Core/Context.h>

#include "legacyMode/playerLogic/lTeam.h"
#include "tacticalMode/gameEvents/tGameEvents.h"
#include "legacyMode/utility/lGeneralMethods.h"

#include "legacyMode/ai/researchProbe.h"

using namespace Urho3D;

void ResearchProbe::RegisterObject(Context* context)
{
	context->RegisterFactory<ResearchProbe>();
}

ResearchProbe::ResearchProbe(Context* context)
	: LogicComponent(context)
{
	SetUpdateEventMask(USE_UPDATE);
}

ResearchProbe::~ResearchProbe()
{
}

void ResearchProbe::Update(F32 timeStep)
{
	LTeam* team = LGeneralMethods::GetOwnerTeam(node_);
	if (!team)
		return;

	// current star system
	PODVector<Node*> starSystemNodes = LGeneralMethods::GetStarSystemNodes(node_);
	for (U32 i = 0; i < starSystemNodes.Size(); i++)
		// TODO: starSystem->starSystemRadius_ too little
		if ((node_->GetWorldPosition2D() - starSystemNodes[i]->GetWorldPosition2D()).Length() < 500.f)
		{
			U32 systemID = starSystemNodes[i]->GetID();
			if (team->IsStarSystemFound(systemID) || team->IsStarSystemSurveyed(systemID))
				continue;

			team->AddSurveyedStarSystem(systemID);

			String infoString = "New star system has been found!";
			SendEvent(RE_INGAME_INFO_STRING, GameInfo::INFO_STRING, infoString);
		}
}
