#pragma once
#include <Urho3D/Core/Object.h>

// NETWORK

// messages to clients
/*
URHO3D_EVENT(RE_NEW_PLAYER, GalaxyNewPlayer)
{
	URHO3D_PARAM(CLIENT_ID, ClientId);
	URHO3D_PARAM(TEAM_NUMBER, TeamNumber);
	URHO3D_PARAM(IS_YOU, IsYou);
}

URHO3D_EVENT(RE_REMOVE_PLAYER, GalaxyRemovePlayer)
{
	URHO3D_PARAM(CLIENT_ID, ClientId);
	URHO3D_PARAM(TEAM_NUMBER, TeamNumber);
}

URHO3D_EVENT(RE_CLIENT_CHANGE_TEAM, GalaxyClientChangeTeam)
{
	URHO3D_PARAM(CLIENT_ID, ClientId);
	URHO3D_PARAM(TEAM_NUMBER, TeamNumber);
}
*/
URHO3D_EVENT(RE_GAME_PARAMETERS_CHANGE, GameParametersChange)
{
	URHO3D_PARAM(GALAXY_TYPE, GalaxyType);
	URHO3D_PARAM(GALAXY_AGE, GalaxyAge);
	URHO3D_PARAM(STARS_COUNT, StarsCount);
	URHO3D_PARAM(ENEMIES_COUNT, EnemiesCount);
}
/*
URHO3D_EVENT(RE_GAME_STARTED, GameStarted)
{
}
*/
// OBJECT
URHO3D_EVENT(RE_OBJECT_BUILD, ObjectBuildEvent)
{
	URHO3D_PARAM(PLAYER_ID, PlayerId);
	URHO3D_PARAM(OBJECT_ID, PlanetId);
	URHO3D_PARAM(BUILDING_NAME, BuildingName);
	URHO3D_PARAM(TARGET_TYPE, TargetType);
}

URHO3D_EVENT(RE_IMPROVE_BUILDING, ImproveBuildingEvent)
{
	URHO3D_PARAM(PLAYER_ID, PlayerId);
	URHO3D_PARAM(OBJECT_ID, PlanetId);
	URHO3D_PARAM(BUILDING_NAME, BuildingName);
	URHO3D_PARAM(TARGET_TYPE, TargetType);
}

URHO3D_EVENT(RE_GET_BUILDING_RESOURCE, GetBuildingResourceEnergyEvent)
{
	URHO3D_PARAM(PLAYER_ID, PlayerId);
	URHO3D_PARAM(OBJECT_ID, ObjectId);
	URHO3D_PARAM(BUILDING_NAME, BuildingName);
	URHO3D_PARAM(RESOURCE_TYPE, ResourceType);
}

// PLANET
// probe
URHO3D_EVENT(RE_CREATE_PROBE, CreateProbeEvent)
{
	URHO3D_PARAM(PLAYER_ID, PlayerId);
}

// equipment
URHO3D_EVENT(RE_CREATE_EQUIPMENT, CreateEquipmentEvent)
{
	URHO3D_PARAM(PLAYER_ID, PlayerId);
	URHO3D_PARAM(EQUIPMENT_TYPE, EquipmentType);
	URHO3D_PARAM(EQUIPMENT_NAME, EquipmentName);
}

// ship factory
URHO3D_EVENT(RE_MOBS_PRODUCTION, ProductionMobsEvent)
{
	URHO3D_PARAM(PLANET_ID, PlanetId);
	URHO3D_PARAM(MOB_NUMBER, MobNumber);
}

// repair factory
URHO3D_EVENT(RE_SHIP_REPAIR, RepairShipEvent)
{
	URHO3D_PARAM(PLAYER_ID, PlayerId);
}

// scrap-metal factory
URHO3D_EVENT(RE_PUT_ON_SCRAP, PutOnScrapEvent)
{
	URHO3D_PARAM(PLAYER_ID, PlayerId);
	URHO3D_PARAM(CARGO_ELEMENT_ID, ElementID);
}

// MAP
URHO3D_EVENT(RE_HYPER_DRIVE, StarSystemDriveEvent)
{
	URHO3D_PARAM(PLAYER_ID, PlayerId);
	URHO3D_PARAM(STAR_SYSTEM_ID, StarSystemId);
}

// CRYSTAL
URHO3D_EVENT(RE_DETECTOR_USE, CrystalDetectorUseEvent)
{
	URHO3D_PARAM(PLAYER_ID, PlayerId);
}
/*
URHO3D_EVENT(RE_EQUIPMENT_MOVE, EquipmentMoveEvent)
{
	URHO3D_PARAM(NODE_ID, NodeId);
	URHO3D_PARAM(NEW_PARENT_ID, NewParentId);
}
*/
// RESEARCH
URHO3D_EVENT(RE_RESEARCH_SELECTION, ResearchSelectionEvent)
{
	URHO3D_PARAM(TEAM_NUMBER, TeamNumber);
	URHO3D_PARAM(RESEARCH_TYPE, ResearchType);
}

URHO3D_EVENT(RE_RESEARCH_COMPLETED, ResearchCompletedEvent)
{
	URHO3D_PARAM(TEAM_NUMBER, TeamNumber);
	URHO3D_PARAM(RESEARCH_TYPE, ResearchType);
}
/*
URHO3D_EVENT(RE_INGAME_INFO_STRING, GameInfo)
{
	URHO3D_PARAM(INFO_STRING, InfoString);
}
*/
URHO3D_EVENT(RE_PLANET_OPEN, PlanetOpen)
{
}
/*
URHO3D_EVENT(RE_OBJECT_DESTROYED, ObjectDestroyed)
{
}

URHO3D_EVENT(RE_GAME_OVER, GameOver)
{
}
*/