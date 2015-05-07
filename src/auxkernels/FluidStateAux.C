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
  params.addCoupledVar("pressure_variable", 1.e6,  "The pressure variable corresponding to the brine phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  params.addCoupledVar("salt_mass_fraction_variable", 0.1, "The salt mass fraction variable.");
  params.addCoupledVar("saturation_variable", 1.0, "The saturation variable corresponding to the brine phase.");
  MooseEnum state_property_enum("liquid_density gas_density liquid_viscosity gas_viscosity");
  params.addRequiredParam<MooseEnum>("state_property_enum", state_property_enum, "The fluid property that this auxillary kernel is to calculate");
  return params;
}

FluidStateAux::FluidStateAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _fluid_property(getUserObject<FluidState>("fluid_state_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _saturation(coupledValue("saturation_variable")),
    _salt_mass_fraction(coupledValue("salt_mass_fraction_variable")),
    state_property_enum(getParam<MooseEnum>("state_property_enum"))
{}

Real FluidStateAux::computeValue()
{
  Real property;

  // Vector of mass fractions of each component in each phase
  std::vector<Real> xmass;
  xmass.push_back(_salt_mass_fraction[_qp]); // Salt mass fraction in liquid
  xmass.push_back(0.);                       // CO2 mass fraction in liquid
  xmass.push_back(0.);                       // Salt mass fraction in gas
  xmass.push_back(1.);                       // CO2 mass fraction in gas

  if (state_property_enum == "liquid_density") {
    property = _fluid_property.liquidDensity(_pressure[_qp], _temperature[_qp], xmass);
  }
  if (state_property_enum == "gas_density") {
    property = _fluid_property.gasDensity(_pressure[_qp], _temperature[_qp], xmass);
  }
  if (state_property_enum == "liquid_viscosity") {
    property = _fluid_property.liquidDensity(_pressure[_qp], _temperature[_qp], xmass);
  }

  return property;
}
