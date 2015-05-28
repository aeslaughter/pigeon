/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidPropertiesCO2.h"

template<>
InputParameters validParams<FluidPropertiesCO2>()
{
  InputParameters params = validParams<FluidProperties>();
  params.addClassDescription("Thermophysical fluid properties of gas phase CO2.");
  return params;
}

FluidPropertiesCO2::FluidPropertiesCO2(const std::string & name, InputParameters parameters) :
  FluidProperties(name, parameters)
{
  _t_c2k = 273.15;
  _Mco2 = 44.0;
}

Real
  FluidPropertiesCO2::density(Real pressure, Real temperature) const

{
  Real tk = temperature + _t_c2k;
  Real tc = std::pow((tk * 1.e-2), 10./3.);
  Real pc = pressure * 1.e-6;

  Real vc1 = 1.8882e-4 * tk;
  Real vc2 = - pc * (8.24e-2 + 1.249e-2 * pc) / tc;

  return pc / (vc1 + vc2);
}

Real
  FluidPropertiesCO2::viscosity(Real pressure, Real temperature) const

{
  Real A[5] = {1357.8, 4.9227, -2.9661e-3, 2.8529e-6, -2.1829e-9};
  Real B[5] = {3918.9, -35.984, 2.5825e-1, -7.1178e-4, 6.9578e-7};
  Real C[5];

  Real pbar = pressure * 1.e-5;
  Real xx = pbar * 0.01;

  if (xx > 1.0) xx = 1.0;

  for (int i = 0; i<5; i++)
    C[i] = A[i] + xx * (B[i] - A[i]);

  Real t2 = temperature * temperature;
  Real t3 = t2 * temperature;
  Real t4 = t3 * temperature;

  return (C[0] + C[1] * temperature + C[2] * t2 + C[3] * t3 + C[4] * t4) * 1.e-8;
}

Real
  FluidPropertiesCO2::dDensity_dP(Real pressure, Real temperature) const
{
  Real tk = temperature + _t_c2k;
  Real tc = std::pow((tk * 1.e-2), 10./3.);
  Real pc = pressure * 1.e-6;

  Real vc1 = 1.8882e-4 * tk;
  Real vc2 = - pc * (8.24e-2 + 1.249e-2 * pc) / tc;
  Real dvc2 = - (8.24e-2 + 2.498e-2 * pc) / tc;

  return (vc1 + vc2 - pc * dvc2) / ((vc1 + vc2) * (vc1 + vc2));
}

std::vector<Real>
  FluidPropertiesCO2::henryConstants() const

{
  std::vector<Real> co2henry;
  co2henry.push_back(-8.55445);
  co2henry.push_back(4.01195);
  co2henry.push_back(9.52345);

  /* Battistelli formulation from TOUGH2
  Real a[6] = {7.83666e7, 1.96025e6, 8.20574e4, -7.40674e2, 2.1838, -2.20999e-3};

  Real co2henry = 0.;

  for(unsigned int i = 0; i< 6; i++)
    co2henry += a[i] * std::pow(temperature, i);
*/
  return co2henry;
}

Real
  FluidPropertiesCO2::partialDensity(Real temperature) const
{
  Real t2 = temperature * temperature;
  Real t3 = t2 * temperature;

  Real V = 37.51 - 9.585e-2 * temperature + 8.74e-4 * t2 - 5.044e-7 * t3;

  return 1000.0 * _Mco2 / V;
}
