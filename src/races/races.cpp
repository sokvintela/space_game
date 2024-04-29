#include "races/races.h"

namespace Races
{
	Urho3D::String GetRaceName(Races race)
	{
		return GetRaceName(static_cast<U32>(race));
	}

	Urho3D::String GetRaceName(U32 raceNumber)
	{
		return raceNames_[raceNumber];
	}
};
