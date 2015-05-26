/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid material for multiphase flow                           */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidStateMaterial.h"

template<>
InputParameters validParams<FluidStateMaterial>()
{
  InputParameters params = validParams<Material>();

  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state model");
  params.addRequiredCoupledVar("liquid_saturation_variable","The liquid saturation variable");
  params.addRequiredCoupledVar("liquid_pressure_variable","The primary pressure variable");
  params.addCoupledVar("temperature_variable", "The temperature variable");

  // Aux variable for gas pressure - must be elemental to couple into material. TODO: gradient of capillary presure uo
  // to use directly in material
  params.addRequiredCoupledVar("gas_pressure_variable", "The gas pressure variable");
  return params;
}

FluidStateMaterial::FluidStateMaterial(const std::string & name,
                                   InputParameters parameters) :
    Material(name, parameters),

    _gravity(getMaterialProperty<RealVectorValue>("gravity")),
    // Declare gas and liquid fluxes (without mobility)
    _gas_flux_no_mobility(declareProperty<RealVectorValue>("gas_flux_no_mobility")),
    _liquid_flux_no_mobility(declareProperty<RealVectorValue>("liqiud_flux_no_mobility")),

    _liquid_saturation(coupledValue("liquid_saturation_variable")),
    _liquid_pressure(coupledValue("liquid_pressure_variable")), // NOTE: Assuming it is liquid pressure at the moment
    _grad_liquid_pressure(coupledGradient("liquid_pressure_variable")),
    _gas_pressure(coupledValue("gas_pressure_variable")),
    _grad_gas_pressure(coupledGradient("gas_pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _fluid_state(getUserObject<FluidState>("fluid_state_uo"))

{
}

void
FluidStateMaterial::computeQpProperties()
{
  Real temperature;

  // Check if the simulation is isothermal
  if (_fluid_state.isIsothermal())
    temperature = _fluid_state.temperature();
  else
    temperature = _temperature[_qp];

// Vector of mass fractions of each component in each phase. TODO: fix this!
  std::vector<Real> xmass;
  xmass.push_back(1.0); // Salt mass fraction in liquid
  xmass.push_back(0.);                       // CO2 mass fraction in liquid
  xmass.push_back(0.);                       // Salt mass fraction in gas
  xmass.push_back(1.);                       // CO2 mass fraction in gas

  _gas_flux_no_mobility[_qp] = _grad_gas_pressure[_qp]+ _fluid_state.density(_gas_pressure[_qp], temperature)[0] *  
                               _gravity[_qp]; 
  _liquid_flux_no_mobility[_qp] = _grad_liquid_pressure[_qp] + _fluid_state.density(_liquid_pressure[_qp], temperature)[1] *
                                  _gravity[_qp];

}
