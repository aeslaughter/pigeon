/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid properties of brine.                                   */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidStateBrineCO2.h"

template<>
InputParameters validParams<FluidStateBrineCO2>()
{
  InputParameters params = validParams<FluidState>();
  params.addClassDescription("Thermophysical fluid state of brine (H20 - NaCl mixtures) and CO2.");
  params.addRequiredParam<UserObjectName>("brine_property_uo", "Name of the User Object defining the brine properties");
  params.addRequiredParam<UserObjectName>("co2_property_uo", "Name of the User Object defining the CO2 properties");
  return params;
}

FluidStateBrineCO2::FluidStateBrineCO2(const std::string & name, InputParameters parameters) :
  FluidState(name, parameters),

  _brine_property(getUserObject<FluidPropertiesBrine>("brine_property_uo")),
  _co2_property(getUserObject<FluidPropertiesCO2>("co2_property_uo"))

{
}

Real
  FluidStateBrineCO2::liquidDensity(Real pressure, Real temperature, std::vector<Real> xmass) const
{
  Real brine_density = _brine_property.density(pressure, temperature, xmass[0]);

  return brine_density;
}

Real
  FluidStateBrineCO2::gasDensity(Real pressure, Real temperature, std::vector<Real> xmass) const
{
  Real co2_density = _co2_property.density(pressure, temperature);

  return co2_density;
}

Real
  FluidStateBrineCO2::liquidViscosity(Real pressure, Real temperature, std::vector<Real> xmass) const
{
  return 0.;
}

Real
  FluidStateBrineCO2::gasViscosity(Real pressure, Real temperature, std::vector<Real> xmass) const
{
  return 0.;
}
