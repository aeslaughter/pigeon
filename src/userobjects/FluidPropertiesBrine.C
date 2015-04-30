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

#include "FluidPropertiesBrine.h"

template<>
InputParameters validParams<FluidPropertiesBrine>()
{
  InputParameters params = validParams<FluidProperties>();
  params.addClassDescription("Thermophysical fluid properties of brine (H20 - NaCl mixtures).");
  params.addRequiredParam<UserObjectName>("water_property_uo", "Name of the User Object defining the water properties");
  return params;
}

FluidPropertiesBrine::FluidPropertiesBrine(const std::string & name, InputParameters parameters) :
  FluidProperties(name, parameters),

  _water_property(getUserObject<FluidPropertiesWater>("water_property_uo"))
{
}

Real
  FluidPropertiesBrine::density(Real pressure, Real temperature, Real xnacl) const
{
  Real n1, n2, n11, n21, n22, Tv;
  Real water_density, density;

  // Molar mass of water and NaCl (kg/mol)
  Real Mwater = 18.015e-3;
  Real Mnacl = 58.443e-3;

  // Need the pressure in bar, not Pa.
  Real pbar = pressure * 1.0e-5;

  // The mole fraction of NaCl is then
  Real Xnacl = xnacl * (xnacl * Mnacl + (1.0 - xnacl) * Mwater) / Mnacl;

  n11 = -54.2958 - 45.7623 * std::exp(-9.44785e-4 * pbar);
  n21 = -2.6142 - 2.39092e-4 * pbar;
  n22 = 0.0356828 + 4.37235e-6 * pbar + 2.0566e-9 * pbar * pbar;

  // The temperature Tv (in bars) where the brine has the same molar volume as pure water
  n1 = n11 * (1.0 - Xnacl);
  n2 = n21 * std::sqrt(Xnacl + n22);

  Tv = n1 + n2 * temperature;

  // The brine density is then given by the molar density of water (convert Tv to C)
  water_density = _water_property.density(pressure, Tv * 1.0e5);

  density = water_density * (xnacl * Mnacl + (1.0 - xnacl) * Mwater) / Mwater;

  return density;
}

Real
  FluidPropertiesBrine::viscosity(Real temperature, Real density, Real xnacl) const

{

  return 0.;
}
