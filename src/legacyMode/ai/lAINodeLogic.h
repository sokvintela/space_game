#pragma once

#include <Urho3D/Math/Vector2.h>

#include "ai/AILogic/AINode.h"

namespace AI
{
namespace Nodes
{
// follow
class FollowZeroPlayer : public AINode
{
public:
	FollowZeroPlayer() = default;
	virtual ~FollowZeroPlayer() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class FollowOwner : public AINode
{
public:
	FollowOwner() = default;
	virtual ~FollowOwner() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class FollowTarget : public AINode
{
public:
	FollowTarget() = default;
	virtual ~FollowTarget() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

// fly to
class FlyToClosestSolidPlanet : public AINode
{
public:
	FlyToClosestSolidPlanet() = default;
	virtual ~FlyToClosestSolidPlanet() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class FlyToTargetPlanet : public AINode
{
public:
	FlyToTargetPlanet() = default;
	virtual ~FlyToTargetPlanet() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class FlyToCrystal : public AINode
{
public:
	FlyToCrystal() = default;
	virtual ~FlyToCrystal() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

// research
class SpiralSearch : public AINode
{
public:
	SpiralSearch();
	virtual ~SpiralSearch() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
private:

	F32 absoluteAngle_;
	Urho3D::Vector2 startingPosition_;

	static F32 spiralCoeff_;
};

// checks
class HasCrystalDetector : public AINode
{
public:
	HasCrystalDetector() = default;
	virtual ~HasCrystalDetector() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class HasResourceOnEFactoryAndCDetector : public AINode
{
public:
	HasResourceOnEFactoryAndCDetector() = default;
	virtual ~HasResourceOnEFactoryAndCDetector() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class HasResourceOnRadar : public AINode
{
public:
	HasResourceOnRadar() = default;
	virtual ~HasResourceOnRadar() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class HasEveryPanetImprovedRadar : public AINode
{
public:
	HasEveryPanetImprovedRadar() = default;
	virtual ~HasEveryPanetImprovedRadar() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

// calculate
class BetterRadarToGetEnergy : public AINode
{
public:
	BetterRadarToGetEnergy() = default;
	virtual ~BetterRadarToGetEnergy() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class PlanetWithoutRadar : public AINode
{
public:
	PlanetWithoutRadar() = default;
	virtual ~PlanetWithoutRadar() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class PlanetWithoutDefenderPlatform : public AINode
{
public:
	PlanetWithoutDefenderPlatform() = default;
	virtual ~PlanetWithoutDefenderPlatform() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

// planet
class BuildEquipmentFactory : public AINode
{
public:
	BuildEquipmentFactory() = default;
	virtual ~BuildEquipmentFactory() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class CreateCrystalDetector : public AINode
{
public:
	CreateCrystalDetector() = default;
	virtual ~CreateCrystalDetector() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class BuildRadar : public AINode
{
public:
	BuildRadar() = default;
	virtual ~BuildRadar() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class TakeRadarEnergy : public AINode
{
public:
	TakeRadarEnergy() = default;
	virtual ~TakeRadarEnergy() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class ImproveRadar : public AINode
{
public:
	ImproveRadar() = default;
	virtual ~ImproveRadar() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class BuildDefenderPlatform : public AINode
{
public:
	BuildDefenderPlatform() = default;
	virtual ~BuildDefenderPlatform() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

// crystal
class UseCrystalDetector : public AINode
{
public:
	UseCrystalDetector() = default;
	virtual ~UseCrystalDetector() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};

class FireIfEnemy : public AINode
{
public:
	FireIfEnemy() = default;
	virtual ~FireIfEnemy() = default;

	std::unique_ptr<AINode> Clone() const override;
	NodeReturnStatus Process(F32 timeStep) override;
};
};
};
