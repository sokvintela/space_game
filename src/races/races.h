#pragma once

#include <Urho3D/Container/Str.h>

#include "utility/simpleTypes.h"

namespace Races
{
	enum Races : U16
	{
		R_Irrs,
		R_Urrs,
		R_Inns,
		R_Onns,
		R_Tirrs,
		R_Chinchi,
		R_Runnes,
		R_Rogges,
		R_Unns,
		R_Xonns,
		R_Otts,
		R_Sovvs,
		R_Ivvs,
		R_Hanns,
		R_Turrs,
		R_Onts,
		R_Yulls,
		R_Yunns,
		R_Yutts,
		R_Yanns,
		R_Yarrs,
		R_Yapps,
		R_Yupps,
		R_Yurrs,
		R_Locks,
		R_Chunchi,
		R_Chanchi,
		R_Chizchi,
		R_Solls,
		R_Sopps,
		R_Ichis,
		R_Random,
		R_Count
	};

	static const char* raceNames_[] = {
		"Irrs",
		"Urrs",
		"Inns",
		"Onns",
		"Tirrs",
		"Chinchi",
		"Runnes",
		"Rogges",
		"Unns",
		"Xonns",
		"Otts",
		"Sovvs",
		"Ivvs",
		"Hanns",
		"Turrs",
		"Onts",
		"Yulls",
		"Yunns",
		"Yutts",
		"Yanns",
		"Yarrs",
		"Yapps",
		"Yupps",
		"Yurrs",
		"Locks",
		"Chunchi",
		"Chanchi",
		"Chizchi",
		"Solls",
		"Sopps",
		"Ichis",
		"Random choice",
		0
	};

	Urho3D::String GetRaceName(Races race);
	Urho3D::String GetRaceName(U32 raceNumber);
};
