#include "PorousMaterial.h"

template<>
InputParameters validParams<PorousMaterial>()
{
	InputParameters params = validParams<Material>();
	params.addRequiredParam<Real>("porosity", "The porosity");
	params.addRequiredParam<Real>("solid_density", "The solid density");
	params.addRequiredParam<Real>("fluid_density", "The fluid density");
	params.addRequiredParam<Real>("solid_heat_capacity", "The solid heat capacity");
	params.addRequiredParam<Real>("fluid_heat_capacity", "The fluid heat capacity");
	params.addRequiredParam<Real>("solid_thermal_conductivity", "The solid thermal conductivity");
	params.addRequiredParam<Real>("fluid_thermal_conductivity", "The fluid thermal conductivity");
	params.addRequiredParam<Real>("fluid_compressibility", "The fluid compressibility");
	params.addRequiredParam<Real>("permeability", "The permeability");
	params.addRequiredParam<Real>("fluid_viscosity", "The fluid viscosity");
	


	return params;
}

PorousMaterial::PorousMaterial(const std::string & name, InputParameters parameters) :
	Material( name, parameters ),
	// Get params from input file
	_porosity(getParam<Real>("porosity")),
	_solid_density(getParam<Real>("solid_density")),
    _fluid_density(getParam<Real>("fluid_density")),
	_solid_heat_capacity(getParam<Real>("solid_heat_capacity")),
	_fluid_heat_capacity(getParam<Real>("fluid_heat_capacity")),
    _solid_thermal_conductivity(getParam<Real>("solid_thermal_conductivity")),
    _fluid_thermal_conductivity(getParam<Real>("fluid_thermal_conductivity")),
    _fluid_compressibility(getParam<Real>("fluid_compressibility")),
    _permeability(getParam<Real>("permeability")),
    _fluid_viscosity(getParam<Real>("fluid_viscosity")),

    // Declare material properties that kernels can use
    _bulk_thermal_conductivity(declareProperty<Real>("bulk_thermal_conductivity")),
    _bulk_specific_heat(declareProperty<Real>("bulk_specific_heat")),
    _fluid_specific_heat(declareProperty<Real>("fluid_specific_heat")),
    _fluid_transient_coeff(declareProperty<Real>("fluid_transient_coeff")),
    _hydraulic_conductivity(declareProperty<Real>("hydraulic_conductivity"))
	
{}

void
PorousMaterial::computeQpProperties()
{
	// Bulk thermal conductivity
	_bulk_thermal_conductivity[_qp] = _porosity * _fluid_thermal_conductivity + (1-_porosity) * _solid_thermal_conductivity;

	// Bulk specific heat
	_bulk_specific_heat[_qp] = _porosity * _fluid_density * _fluid_heat_capacity + (1-_porosity) * _solid_density * _solid_heat_capacity;

	// Fluid specific heat
	_fluid_specific_heat[_qp] = _fluid_density *_fluid_heat_capacity;

	// Fluid transient coeff
	_fluid_transient_coeff[_qp] = _porosity *_fluid_density *_fluid_compressibility;

	// Hydraulic conductivity
	_hydraulic_conductivity[_qp] = _permeability * _fluid_density / _fluid_viscosity;
}