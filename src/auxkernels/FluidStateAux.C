/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate fluid property using the given */
/* Fluid State UserObject for multiphase flow in porous         */
/* media.                                                       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidStateAux.h"

template<>
InputParameters validParams<FluidStateAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state");
  params.addCoupledVar("pressure_variable", 1.e6,  "The pressure variable corresponding to the phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  params.addCoupledVar("saturation_variable", 1.0, "The saturation variable corresponding to the phase.");
  MooseEnum state_property_enum("liquid_density gas_density liquid_viscosity gas_viscosity mass_fraction saturation pressure liquid_relperm gas_relperm");
  params.addRequiredParam<MooseEnum>("state_property_enum", state_property_enum, "The fluid property that this auxillary kernel is to calculate");
  return params;
}

FluidStateAux::FluidStateAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _saturation(coupledValue("saturation_variable")),
    _state_property_enum(getParam<MooseEnum>("state_property_enum"))
{}

Real FluidStateAux::computeValue()
{
  Real temperature, property;

  // Vector of mass fractions of each component in each phase
  std::vector<Real> xmass;
  xmass.push_back(1.0); 		// Salt mass fraction in liquid
  xmass.push_back(0.);                       // CO2 mass fraction in liquid
  xmass.push_back(0.);                       // Salt mass fraction in gas
  xmass.push_back(1.);                       // CO2 mass fraction in gas

  // If the simulation is isothermal, take the temperature from the FluidState UserObject.
  if (_fluid_state.isIsothermal())
    temperature = _fluid_state.temperature();
  else
    temperature = _temperature[_qp];

  if (_state_property_enum == "liquid_density") {
    property = _fluid_state.density(_pressure[_qp], temperature)[0];
  }
  if (_state_property_enum == "gas_density") {
    property = _fluid_state.density(_pressure[_qp], temperature)[1];
  }
  if (_state_property_enum == "liquid_viscosity") { 
    property = _fluid_state.viscosity(_pressure[_qp], temperature)[0];
  }
  if (_state_property_enum == "gas_viscosity") { 
    property = _fluid_state.viscosity(_pressure[_qp], temperature)[1];
  }
  if (_state_property_enum == "saturation") { 
    property = _fluid_state.saturation(_saturation[_qp]);
  }
  if (_state_property_enum == "pressure") { 
    property = _fluid_state.pressure(_pressure[_qp], _saturation[_qp]);
  }
  if (_state_property_enum == "liquid_relperm") { 
    property = _fluid_state.relativePermeability(_saturation[_qp])[0];
  }
  if (_state_property_enum == "gas_relperm") { 
    property = _fluid_state.relativePermeability(_saturation[_qp])[1];
  }


  return property;
}
