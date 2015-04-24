/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid properties of water                                    */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidPropertiesWater.h"

template<>
InputParameters validParams<FluidPropertiesWater>()
{
  InputParameters params = validParams<FluidProperties>();
  params.addClassDescription("Thermophysical fluid properties of water.");
  return params;
}

FluidPropertiesWater::FluidPropertiesWater(const std::string & name, InputParameters parameters) :
  FluidProperties(name, parameters)
{
}

/// Density of water
Real
  FluidPropertiesWater::fluidDensity(Real pressure, Real temperature) const

{
// Hard code constant bulk modulus pressure dependence.
  Real waterdensity = 996.53 * std::exp(pressure /(2.2e9)); // Density at P = 20MPa, T = 50C

  return waterdensity;
}

/// Viscosity of water
Real
  FluidPropertiesWater::fluidViscosity(Real pressure, Real temperature) const

{

  Real waterviscosity = 550.56e-6; // Viscosity at P = 20MPa, T = 50C

  return waterviscosity;
}

/// Henry coefficient of CO2 as a function of temperature.
/*
Real
  FluidPropertiesWater::fluidHenryCoefficient(Real temperature) const

{

  Real co2henry = 200.0e+6; // Henry coefficient at T = 50C

  return co2henry;
}*/
