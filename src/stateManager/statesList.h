#pragma once

#include "utility/simpleTypes.h"

namespace GameStates
{
	enum GameState : U32
	{
		START_MAIN_MENU,
		MENU_SINGLE_GAME,
		MENU_MULTI_GAME,
		MENU_PROPERTIES,
		VIDEO_PROPERTIES,
		AUDIO_PROPERTIES,
		CONTROLS_PROPERTIES,
		LOADING_GAME,
		CHOOSE_RACE,
		LSPACE,
		TSPACE,
		GALAXY_SETUP,
		MULTI_GALAXY_SETUP,
		MENU_CONNECT_STATE,
		GALAXY_STATE,
		MULTI_FAST,
		Count
	};
};