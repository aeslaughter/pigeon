/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid properties of supercritical CO2.                       */
/* porous media.                                                */
/*                                                              */
/* Density and viscosity correlation from Liang-Biao Ouyang,    */
/* New correlations for predicting the density and viscosity of */
/* supercritical carbon dioxide under conditions expected in    */
/* carbon capture and sequestration operations. The Open Pet.   */
/* Eng. J., 4, 13-21, 2011.                                     */
/*                                                              */
/* Henry coefficient from Battistelli et al, A fluid property   */
/* module for the TOUGH2 simulator for saline brines with       */
/* non-condensible gas, Proc. 18th Workshop Geothermal Res. Eng.*/
/* 1993.                                                        */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidPropertiesCO2.h"

template<>
InputParameters validParams<FluidPropertiesCO2>()
{
  InputParameters params = validParams<FluidProperties>();
  params.addClassDescription("Thermophysical fluid properties of CO2.");
  return params;
}

FluidPropertiesCO2::FluidPropertiesCO2(const std::string & name, InputParameters parameters) :
  FluidProperties(name, parameters)
{
}

/// Density of supercritical CO2
Real
  FluidPropertiesCO2::density(Real pressure, Real temperature) const

{
// Hard code constant bulk modulus pressure dependence.
  Real co2density = 784.29 * std::exp(pressure /(2.0e9)); // Density at P = 20MPa, T = 50C

  return co2density;
}

/// Viscosity of supercritical CO2
Real
  FluidPropertiesCO2::viscosity(Real pressure, Real temperature) const

{

  Real co2viscosity = 68.674e-6; // Viscosity at P = 20MPa, T = 50C

  return co2viscosity;
}
/*
/// Henry coefficient of CO2 as a function of temperature.
Real
  FluidPropertiesCO2::henry(Real temperature) const

{

  Real co2henry = 200.0e+6; // Henry coefficient at T = 50C

  return co2henry;
}*/
