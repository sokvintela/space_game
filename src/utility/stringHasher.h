#pragma once

#include <Urho3D/Container/Str.h>

struct StringHasher
{
	std::size_t operator()(const Urho3D::String& str) const
	{
		return str.ToHash();
	}
};
