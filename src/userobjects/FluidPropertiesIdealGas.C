/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidPropertiesIdealGas.h"

template<>
InputParameters validParams<FluidPropertiesIdealGas>()
{
  InputParameters params = validParams<FluidProperties>();
  params.addClassDescription("Thermophysical fluid properties of gas phase CO2.");
  return params;
}

FluidPropertiesIdealGas::FluidPropertiesIdealGas(const std::string & name, InputParameters parameters) :
  FluidProperties(name, parameters)
{
  _t_c2k = 273.15;
  _R = 8.3144621;
}

Real
FluidPropertiesIdealGas::density(Real pressure, Real temperature, Real molar_mass) const

{
  return pressure * molar_mass / (_R * (temperature + _t_c2k)) / 1000.;
}


Real
FluidPropertiesIdealGas::dDensity_dP(Real temperature, Real molar_mass) const
{
  return molar_mass / (_R * (temperature + _t_c2k));
}

Real
FluidPropertiesIdealGas::dDensity_dT(Real pressure, Real temperature, Real molar_mass) const
{
  Real tk = temperature + _t_c2k;
  return - pressure * molar_mass / (_R * tk * tk);
}
