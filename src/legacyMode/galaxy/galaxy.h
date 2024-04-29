#pragma once

#include <Urho3D/Scene/LogicComponent.h>

#include "utility/simpleTypes.h"

using namespace Urho3D;

class Galaxy : public LogicComponent
{
	URHO3D_OBJECT(Galaxy, LogicComponent);

public:

	struct Cluster
	{
		U32 x;
		U32 y;
	};
	HashMap<S32, HashMap<S32, Cluster>> clusters_;

	enum Type
	{
		Elliptic,
		Spiral2,
		TypeCount
	};

	enum Age
	{
		Young,
		Medium,
		Old,
		AgeCount
	};

	static void RegisterObject(Context* context);
	bool SaveDefaultAttributes() const override { return true; }

	Galaxy(Context* context);
	~Galaxy() = default;

	void Generate(Type type, Age age, U32 numberOfStars);
	Vector2 GenerateSystemPosition(U32 number) const;

	void CreatePlayers(U32 numberAi);
	void CreatePlayer(U32 playerNumber, U32 teamNumber, bool isAI = false);
	void SelectStartSystemForPlayer(Node* playerNode);

	Node* GetPlayersNode(U32 teamID) const;

	F32 galaxyRadius_;

private:
	F32 step_;
	F32 spiralCoeff_;

	Type type_;
	Age age_;

	U32 numberOfStars_;
	U32 numberOfPlayers_;

	/// <index, system number>
	Vector<U32> startingStarSystems_;
	HashSet<U16> startingRaces_;
};
