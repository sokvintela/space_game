#pragma once

/// Key pressed.
URHO3D_EVENT(G_STATE_CHANGE, GameChangeStateEvent)
{
	URHO3D_PARAM(P_STATE, cmdType);               // States
	URHO3D_PARAM(P_OBJ, sender);                // class
	URHO3D_PARAM(P_DESTROY_PREVIOS_STATE, destroyOldState);
}

URHO3D_EVENT(G_NEW_STATE, GameNewStateEvent)
{
	URHO3D_PARAM(P_STATE, cmdType);               // States
	URHO3D_PARAM(P_OBJ, sender);                // class
}

URHO3D_EVENT(G_STATE_EXIT_STATE, GameExitStateEvent)
{
}

URHO3D_EVENT(G_STATE_EXIT_GAME, GameExitEvent)
{
}
