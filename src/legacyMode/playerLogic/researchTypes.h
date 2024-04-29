#pragma once

static const char* researchTypeNames[] = {
	"Base",
	"ElectromagneticFields",
	"Lasers",
	"EnergyShielding",
	"EnergyElements",
	"StrongEMFields",
	"Plasma",
	"QuantumPhysics",
	"PhaseDetectors",
	"SuperlightTravel",
	// TODO
	"TODO_EngineFactory",
	"TODO_EquipmentFactory",
	"TODO_MetalExtractionFactory",
	"TODO_OrbitalFactory",
	"TODO_ProbesFactory",
	"TODO_RepairFactory",
	"TODO_ScrapMetalFactory",
	"TODO_ShipFactory",
	"TODO_WeaponFactory",
	"TODO_ScienceLaboratory",
	"TODO_DefenderPlatform",
	"TODO_repairDrone",
	"TODO_chemicalWeapon",
	"TODO_railgunWeapon",
	"TODO_rockets",
	"TODO_chemicalFieldGenerators",
	"TODO_ionFieldGenerators",
	"TODO_nuclearFieldGenerators",
	"TODO_photonFieldGenerators",
	"TODO_plasmaFieldGenerators",
	"TODO_sunFieldGenerators",
	"TODO_chemicalHyperEngine",
	"TODO_ionHyperEngine",
	"TODO_nuclearHyperEngine",
	"TODO_photonHyperEngine",
	"TODO_plasmaHyperEngine",
	"TODO_sunHyperEngine",
	"TODO_chemicalShipEngine",
	"TODO_nuclearHyperEngine",
	"TODO_photonShipEngine",
	"TODO_plasmaShipEngine",
	"TODO_sunShipEngine",
	0
};

namespace Research
{
	enum ResearchType
	{
		Base,
		ElectromagneticFields,
		Lasers,
		EnergyShielding,
		EnergyElements,
		StrongEMFields,
		Plasma,
		QuantumPhysics,
		PhaseDetectors,
		SuperlightTravel,
		// planet factories
		TODO_EngineFactory,
		TODO_EquipmentFactory,
		TODO_MetalExtractionFactory,
		TODO_OrbitalFactory,
		TODO_ProbesFactory,
		TODO_RepairFactory,
		TODO_ScrapMetalFactory,
		TODO_ShipFactory,
		TODO_WeaponFactory,
		// neutron star buildings
		TODO_ScienceLaboratory,
		// common buildings
		TODO_DefenderPlatform,
		// eqipment
		TODO_repairDrone,
		// weapon
		TODO_chemicalWeapon,
		TODO_railgunWeapon,
		TODO_rockets,
		// energy field generators
		TODO_chemicalFieldGenerators,
		TODO_ionFieldGenerators,
		TODO_nuclearFieldGenerators,
		TODO_photonFieldGenerators,
		TODO_plasmaFieldGenerators,
		TODO_sunFieldGenerators,
		// hyper drives
		TODO_chemicalHyperEngine,
		TODO_ionHyperEngine,
		TODO_nuclearHyperEngine,
		TODO_photonHyperEngine,
		TODO_plasmaHyperEngine,
		TODO_sunHyperEngine,
		// engines
		TODO_chemicalShipEngine,
		TODO_nuclearShipEngine,
		TODO_photonShipEngine,
		TODO_plasmaShipEngine,
		TODO_sunShipEngine,
		Count
	};
};
