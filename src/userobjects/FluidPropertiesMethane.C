/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidPropertiesMethane.h"

template<>
InputParameters validParams<FluidPropertiesMethane>()
{
  InputParameters params = validParams<FluidPropertiesIdealGas>();
  params.addClassDescription("Thermophysical fluid properties of gas phase CH4.");
  return params;
}

FluidPropertiesMethane::FluidPropertiesMethane(const InputParameters & parameters) :
  FluidPropertiesIdealGas(parameters)
{
  _t_c2k = 273.15;
  _Mch4 = 16.0425;

}

Real
FluidPropertiesMethane::molarMass() const
{
  return _Mch4;
}

Real
FluidPropertiesMethane::density(Real pressure, Real temperature, Real xmass) const

{
  return FluidPropertiesIdealGas::density(pressure, temperature, _Mch4);
}

Real
FluidPropertiesMethane::viscosity(Real pressure, Real temperature, Real density, Real xmass) const
{
  Real a[6] = {2.968267e-1, 3.711201e-2, 1.218298e-5, -7.02426e-8, 7.543269e-11,
    -2.7237166e-14};

  Real viscosity;
  Real tk = temperature + _t_c2k;
  Real tk2 = tk * tk;
  Real tk3 = tk2 * tk;
  Real tk4 = tk3 * tk;
  Real tk5 = tk4 * tk;

  viscosity = a[0] + a[1] * tk + a[2] * tk2 + a[3] * tk3 + a[4] * tk4 + a[5] * tk5;

  return viscosity * 1.e-6;
}

Real
FluidPropertiesMethane::dDensity_dP(Real pressure, Real temperature, Real xmass) const
{
  return FluidPropertiesIdealGas::dDensity_dP(pressure, temperature, _Mch4);
}

Real
FluidPropertiesMethane::dDensity_dT(Real pressure, Real temperature, Real xmass) const
{
  return FluidPropertiesIdealGas::dDensity_dT(pressure, temperature, _Mch4);
}

Real
FluidPropertiesMethane::dViscosity_dDensity(Real pressure, Real temperature, Real density, Real xmass) const
{
  /**
   * The viscosity correlation is a function of temperature only. Therefore, use the chain rule to
   * calculate d(viscosity)/d(density)
   * Note: if d(density)/d(temperature) = 0, so should d(viscosity)/d(density)
   */
  Real dmu_drho;
  Real drho_dt = dDensity_dT(pressure, temperature);

  if (drho_dt != 0.0)
    dmu_drho = dViscosity_dT(pressure, temperature) / drho_dt;

  return dmu_drho;
}

Real
FluidPropertiesMethane::dViscosity_dT(Real pressure, Real temperature) const
{
  Real a[6] = {2.968267e-1, 3.711201e-2, 1.218298e-5, -7.02426e-8, 7.543269e-11,
    -2.7237166e-14};

  Real dviscosity;
  Real tk = temperature + _t_c2k;
  Real tk2 = tk * tk;
  Real tk3 = tk2 * tk;
  Real tk4 = tk3 * tk;

  dviscosity = a[1] + 2.0 * a[2] * tk + 3.0 * a[3] * tk2 + 4.0 * a[4] * tk3 + 5.0 * a[5] * tk4;

  return dviscosity * 1.e-6;
}

std::vector<Real>
FluidPropertiesMethane::henryConstants() const

{
  std::vector<Real> ch4henry;
  ch4henry.push_back(-10.44708);
  ch4henry.push_back(4.66491);
  ch4henry.push_back(12.12986);

  return ch4henry;
}
