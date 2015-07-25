/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
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

FluidPropertiesBrine::FluidPropertiesBrine(const InputParameters & parameters) :
  FluidProperties(parameters),

  _water_property(getUserObject<FluidPropertiesWater>("water_property_uo"))
{
  /// Reference constants used in to calculate thermophysical properties of water.
  _Mh2o = _water_property.molarMass();
  _Mnacl = 58.443e-3;
  _t_c2k = 273.15;
}

Real
FluidPropertiesBrine::molarMass() const
{
  return _Mnacl; //kg/mol
}

Real
FluidPropertiesBrine::density(Real pressure, Real temperature, Real xnacl) const
{
  Real n1, n2, n11, n12, n1x1, n20, n21, n22, n23, n2x1, Tv;
  Real water_density, density;

  // The correlation requires the pressure in bar, not Pa.
  Real pbar = pressure * 1.0e-5;
  Real pbar2 = pbar * pbar;
  Real pbar3 = pbar2 * pbar;

  // The correlation requires mole fraction. First calculate the average molar mass
  // from the mass fraction, then compute the mole fraction
  Real Mbrine = 1.0 / (xnacl / _Mnacl + (1.0 - xnacl) / _Mh2o);
  Real Xnacl = xnacl / _Mnacl * Mbrine;

  n11 = -54.2958 - 45.7623 * std::exp(-9.44785e-4 * pbar);
  n21 = -2.6142 - 2.39092e-4 * pbar;
  n22 = 0.0356828 + 4.37235e-6 * pbar + 2.0566e-9 * pbar2;
  n1x1 = 330.47 + 0.942876 * std::sqrt(pbar) + 0.0817193 * pbar - 2.47556e-8 * pbar2
         + 3.45052e-10 * pbar3;
  n2x1 = -0.0370751 + 0.00237723 * std::sqrt(pbar) + 5.42049e-5 * pbar
         + 5.84709e-9 * pbar2 - 5.99373e-13 * pbar3;
  n12 = - n1x1 - n11;
  n20 = 1.0 - n21 * std::sqrt(n22);
  n23 = n2x1 - n20 -n21 * std::sqrt(1.0 + n22);

  // The temperature Tv (C) where the brine has the same molar volume as pure water
  n1 = n1x1 + n11 * (1.0 - Xnacl) + n12 * (1.0 - Xnacl) * (1.0 - Xnacl);
  n2 = n20 + n21 * std::sqrt(Xnacl + n22) + n23 * Xnacl;
  Tv = n1 + n2 * temperature;

  // The brine density is then given by the molar density of water
  water_density = _water_property.density(pressure, Tv);

  density = water_density * Mbrine / _Mh2o;

  return density;
}

Real
FluidPropertiesBrine::viscosityDens(Real temperature, Real density, Real xnacl) const
{
  // Correlation requires molar concentration (mol/kg)
  Real mol = xnacl / ((1.0 - xnacl) * _Mnacl);
  Real mol2 = mol * mol;
  Real mol3 = mol2 * mol;

  Real a = 1.0 + 0.0816 * mol + 0.0122 * mol2 + 0.128e-3 * mol3 + 0.629e-3 * temperature
           * (1.0 - std::exp(-0.7 * mol));

  // The brine viscosity is then given by a multiplied by the viscosity of pure water. Note
  // that the density is the density of pure water.
  // TODO: In EOS code, add water viscosity as an input parameter so that it is not calculated
  // more than once per qp per iteration.
  return a * _water_property.viscosity(temperature, density);
}

Real
FluidPropertiesBrine::viscosity(Real pressure, Real temperature, Real xnacl) const
{
  // Calculate the water density
  Real water_density = _water_property.density(pressure, temperature);

  return viscosityDens(temperature, water_density, xnacl);
}

Real
FluidPropertiesBrine::haliteDensity(Real pressure, Real temperature) const
{
  // Correlation needs pressure in bar
  Real pbar = pressure * 1.e-5;

  // Halite density at 0 Pa
  Real density0 = 2.17043e3 - 2.4599e-1 * temperature - 9.5797e-5 * temperature * temperature;

  // Halite density as a function of pressure
  Real l = 5.727e-3 + 2.715e-3 * std::exp(temperature / 733.4);

  return density0 + l * pbar;
}

Real
FluidPropertiesBrine::haliteSolubility(Real temperature) const
{
  Real solubility = (26.18 + 7.2e-3 * temperature + 1.06e-4 * temperature * temperature)/100.;

  return solubility;
}

Real
FluidPropertiesBrine::pSat(Real temperature, Real xnacl) const
{
  // Temperature in K
  Real tk = temperature + _t_c2k;

  // Correlation requires molar concentration (mol/kg)
  Real mol = xnacl / ((1.0 - xnacl) * _Mnacl);
  Real mol2 = mol * mol;
  Real mol3 = mol2 * mol;

  Real a = 1.0 + 5.93582e-6 * mol - 5.19386e-5 * mol2 + 1.23156e-5 * mol3;
  Real b = 1.1542e-6 * mol + 1.41254e-7 * mol2 - 1.92476e-8 * mol3 - 1.70717e-9 * mol * mol3
           + 1.0539e-10 * mol2 * mol3;

  // The temperature of pure water at the same pressure as the brine is given by
  // Note that this correlation uses temperature in K, not C
  Real th20 = std::exp(std::log(tk) / (a + b * tk)) - _t_c2k;

  // The brine vapour pressure is then found by evaluating the saturation pressure for pure water
  // using this effective temperature

  return _water_property.pSat(th20);
}

Real
FluidPropertiesBrine::dDensity_dP(Real pressure, Real temperature, Real xnacl) const
{
  return 0.; //TODO: implement brine density derivative wrt P
}
