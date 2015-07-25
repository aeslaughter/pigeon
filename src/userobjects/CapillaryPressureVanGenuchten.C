/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "CapillaryPressureVanGenuchten.h"

template<>
InputParameters validParams<CapillaryPressureVanGenuchten>()
{
  InputParameters params = validParams<CapillaryPressure>();
  params.addRequiredRangeCheckedParam<Real>("sat_lr", "sat_lr >= 0 & sat_lr <= 1", "Liquid residual saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("sat_ls", "sat_ls >= 0 & sat_ls <= 1", "Liquid fully saturated saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("m", "m >= 0 & m <= 1", "van Genuchten exponent m.");
  params.addRequiredRangeCheckedParam<Real>("cp_max", "cp_max > 0", "Maximum capillary pressure");
  params.addRequiredRangeCheckedParam<Real>("p0", "p0 > 0", "P0");
  params.addParam<Real>("delta_s", 0.01, "The interpolation window half-width");
  params.addClassDescription("van Genuchten form of capillary pressure.");
  return params;
}

CapillaryPressureVanGenuchten::CapillaryPressureVanGenuchten(const InputParameters & parameters) :
  CapillaryPressure(parameters),
  _sat_lr(getParam<Real>("sat_lr")),
  _sat_ls(getParam<Real>("sat_ls")),
  _m(getParam<Real>("m")),
  _cp_max(getParam<Real>("cp_max")),
  _p0(getParam<Real>("p0")),
  _delta_s(getParam<Real>("delta_s"))
{
}

Real
CapillaryPressureVanGenuchten::capillaryPressure(Real sat_liq) const
{
  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);
  Real cp;

 if (sat_liq < 0.0 )
    sat_liq = 0.0;
  if (sat_liq > 1.0)
    sat_liq = 1.0;

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("CapillaryPressureVanGenuchten: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  if (sat_eff > 1.0)
    sat_eff = 1.0; // Restrict sat_eff to be less than or equal to unity

  // Locate the point where the cubic spline will be used
  Real sat1 = cutoffIntercept();
  Real sat0 = std::max(0.0, sat1 - 2.0 * _delta_s); // Make sure this isn't negative

  // The point where the cubic spline will be used near full saturation
  Real sat2 = _sat_ls - _delta_s;
  Real sat3 = std::min(1.0, sat2 + 2.0 * _delta_s); // Make sure this <= 1

  if (sat_liq <= sat0)
    cp = - _cp_max; // If sat_eff <= _sat_lr, Pc = cp_max.

  else if (sat0 < sat_liq && sat_liq < sat1)
    cp = cSpline0(sat_liq, sat0, sat1);

  else if (sat1 <= sat_liq && sat_liq <= sat2)
    cp = capillaryPressureUM(sat_liq);

  else if (sat2 < sat_liq && sat_liq < sat3)
    cp = cSpline1(sat_liq, sat2, sat3);

  else
    cp = 0.0;

  // Just in case, constrain the capillary pressure cp so that - _cp_max <= cp <= 0
  if (cp < -_cp_max)
    cp = -_cp_max;
  if (cp > 0.0)
    cp = 0.0;

  return cp;
}

Real
CapillaryPressureVanGenuchten::dCapillaryPressure(Real sat_liq) const
{
  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);
  Real dcp;

 if (sat_liq < 0.0 )
   sat_liq = 0.0;
 if (sat_liq > 1.0)
   sat_liq = 1.0;

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("CapillaryPressureVanGenuchten: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  if (sat_eff > 1.0)
    sat_eff = 1.0; // Restrict sat_eff to be less than or equal to unity

  // Locate the point where the cubic spline will be unsigned
  Real sat1 = cutoffIntercept();
  Real sat0 = std::max(0.0, sat1 - 2.0 * _delta_s); // Make sure this isn't negative

  // The point where the cubic spline will be used near full saturation
  Real sat2 = _sat_ls - _delta_s;
  Real sat3 = std::min(1.0, sat2 + 2.0 * _delta_s); // Make sure this <= 1

  if (sat_liq <= sat0)
    dcp = 0; // If sat_eff <= _sat_lr, dPc = 0.

  else if (sat0 < sat_liq && sat_liq < sat1)
    dcp = dCSpline0(sat_liq, sat0, sat1);

  else if (sat1 <= sat_liq && sat_liq <= sat2)
    dcp = dCapillaryPressureUM(sat_liq);

  else if (sat2 < sat_liq && sat_liq < sat3)
    dcp = dCSpline1(sat_liq, sat2, sat3);

  else
    dcp = 0.0;

  return dcp;
}

Real
CapillaryPressureVanGenuchten::d2CapillaryPressure(Real sat_liq) const
{
  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);
  Real d2cp;
  Real cp  = capillaryPressure(sat_liq);
  Real a;

  // Check if Pc is equal to - _cp_max, and if it is, set the derivative equal to zero.
  if (cp == - _cp_max)
    return 0.0;

  // Locate the point where the cubic spline will be unsigned
  Real sat1 = cutoffIntercept();
  Real sat0 = std::max(0.0, sat1 - 2.0 * _delta_s); // Make sure this isn't negative

  // The point where the cubic spline will be used near full saturation
  Real sat2 = _sat_ls - _delta_s;
  Real sat3 = std::min(1.0, sat2 + 2.0 * _delta_s); // Make sure this <= 1

  if (sat_liq <= sat0)
    d2cp = 0; // If sat_eff <= _sat_lr, d2Pc = 0.

  else if (sat0 < sat_liq && sat_liq < sat1)
    d2cp = d2CSpline0(sat_liq, sat0, sat1);

  else if (sat1 <= sat_liq && sat_liq <= sat2)
    d2cp = d2CapillaryPressureUM(sat_liq);

  else if (sat2 < sat_liq && sat_liq < sat3)
    d2cp = d2CSpline1(sat_liq, sat2, sat3);

  else
    d2cp = 0.0;

  return d2cp;
}

Real
CapillaryPressureVanGenuchten::capillaryPressureUM(Real sat_liq) const
{
  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);
  Real cp;

  cp = - _p0 * std::pow(std::pow(sat_eff, -1.0/_m) - 1.0, 1.0 - _m);

 return cp;
}

Real
CapillaryPressureVanGenuchten::dCapillaryPressureUM(Real sat_liq) const
{
  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);
  Real dcp;

  dcp = _p0 * (1.0 - _m) * std::pow(std::pow(sat_eff, -1.0/_m) - 1.0, -_m) * std::pow(sat_eff, -1.0-1.0/_m) / (_m * (_sat_ls - _sat_lr));

  return dcp;
}

Real
CapillaryPressureVanGenuchten::d2CapillaryPressureUM(Real sat_liq) const
{
  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);
  Real d2cp;
  Real a;

  a = std::pow(sat_eff, -1.0 / _m) - 1.0;
  d2cp = std::pow(a, -1.0 - _m) * std::pow(sat_eff, -2.0 - 2.0 / _m) - ((1.0 + _m) / _m) *std::pow(a, - _m) *
    std::pow(sat_eff, -1.0 / _m - 2.0);
  d2cp *= _p0 * (1.0 - _m) / _m / (_sat_ls - _sat_lr) / (_sat_ls - _sat_lr);

  return d2cp;
}

Real
CapillaryPressureVanGenuchten::cutoffIntercept() const
{
  Real sc = std::pow(1.0 + std::pow(_cp_max/_p0, 1.0 / (1.0 - _m)), -_m);

  return sc * (_sat_ls - _sat_lr) + _sat_lr + _delta_s;
}

Real
CapillaryPressureVanGenuchten::cSpline0(Real sat_liq, Real sat0, Real sat1) const
{
  Real a, b, c, d;
  Real den = std::pow(sat0 - sat1, 3.0);
  Real pc1 = capillaryPressureUM(sat1);
  Real dpc1 = dCapillaryPressureUM(sat1);
  Real sat02 = std::pow(sat0, 2.0);

  a = (pc1 * sat02 * (sat0 - 3 * sat1) + sat1 * (_cp_max * sat1 * (-3.0 * sat0 + sat1) +
    dpc1 * sat02 *(-sat0 + sat1))) / den;
  b = (sat0 * (6.0 * (pc1 + _cp_max) * sat1 + dpc1 * (sat0 - sat1) * (sat0 + 2.0 * sat1))) / den;
  c = (-3.0 * pc1 *(sat0 + sat1) - 3.0 * _cp_max * (sat0 + sat1) + dpc1 * (-2.0 * sat02 +
    sat0 * sat1 + std::pow(sat1, 2.0))) / den;
  d = (2.0 * pc1 + 2.0 * _cp_max + dpc1 * (sat0 - sat1)) / den;

  return a + b * sat_liq + c * sat_liq * sat_liq + d * sat_liq * sat_liq * sat_liq;
}

Real
CapillaryPressureVanGenuchten::dCSpline0(Real sat_liq, Real sat0, Real sat1) const
{
  Real b, c, d;
  Real den = std::pow(sat0 - sat1, 3.0);
  Real pc1 = capillaryPressureUM(sat1);
  Real dpc1 = dCapillaryPressureUM(sat1);
  Real sat02 = std::pow(sat0, 2.0);

  b = (sat0 * (6.0 * (pc1 + _cp_max) * sat1 + dpc1 * (sat0 - sat1) * (sat0 + 2.0 * sat1))) / den;
  c = (-3.0 * pc1 *(sat0 + sat1) - 3.0 * _cp_max * (sat0 + sat1) + dpc1 * (-2.0 * sat02 +
    sat0 * sat1 + std::pow(sat1, 2.0))) / den;
  d = (2.0 * pc1 + 2.0 * _cp_max + dpc1 * (sat0 - sat1)) / den;

  return b + 2.0 * c * sat_liq + 3.0 * d * sat_liq * sat_liq;
}

Real
CapillaryPressureVanGenuchten::d2CSpline0(Real sat_liq, Real sat0, Real sat1) const
{
  Real c, d;
  Real den = std::pow(sat0 - sat1, 3.0);
  Real pc1 = capillaryPressureUM(sat1);
  Real dpc1 = dCapillaryPressureUM(sat1);

  c = (-3.0 * pc1 * (sat0 + sat1) - 3.0 * _cp_max * (sat0 + sat1) + dpc1 * (-2.0 * std::pow(sat0, 2.0) +
    sat0 * sat1 + std::pow(sat1, 2.0))) / den;
  d = (2.0 * pc1 + 2.0 * _cp_max + dpc1 * (sat0 - sat1)) / den;

  return 2.0 * c + 6.0 * d * sat_liq;
}

Real
CapillaryPressureVanGenuchten::cSpline1(Real sat_liq, Real sat2, Real sat3) const
{
  Real a, b, c, d;
  Real den = std::pow(sat2 - sat3, 3.0);
  Real pc2 = capillaryPressureUM(sat2);
  Real dpc2 = dCapillaryPressureUM(sat2);

  a = (std::pow(sat3, 2.0) * (pc2 * (3.0 * sat2 - sat3) + dpc2 * sat2 * (-sat2 + sat3))) / den;
  b = - (sat3 *(6.0 * pc2 * sat2 + dpc2 * (-2.0 * std::pow(sat2, 2.0) + sat2 * sat3 + std::pow(sat3,2.0)))) / den;
  c = (3.0 * pc2 * (sat2 + sat3) - dpc2 * (sat2 - sat3) * (sat2 + 2.0 * sat3)) / den;
  d = (-2.0 * pc2 + dpc2 * (sat2 - sat3)) / den;

  return a + b * sat_liq + c * sat_liq * sat_liq + d * sat_liq * sat_liq * sat_liq;
}

Real
CapillaryPressureVanGenuchten::dCSpline1(Real sat_liq, Real sat2, Real sat3) const
{
  Real b, c, d;
  Real den = std::pow(sat2 - sat3, 3.0);
  Real pc2 = capillaryPressureUM(sat2);
  Real dpc2 = dCapillaryPressureUM(sat2);

  b = - (sat3 *(6.0 * pc2 * sat2 + dpc2 * (-2.0 * std::pow(sat2, 2.0) + sat2 * sat3 + std::pow(sat3,2.0)))) / den;
  c = (3.0 * pc2 * (sat2 + sat3) - dpc2 * (sat2 - sat3) * (sat2 + 2.0 * sat3)) / den;
  d = (-2.0 * pc2 + dpc2 * (sat2 - sat3)) / den;

  return b  + 2.0 * c * sat_liq + 3.0 * d * sat_liq * sat_liq;
}

Real
CapillaryPressureVanGenuchten::d2CSpline1(Real sat_liq, Real sat2, Real sat3) const
{
  Real c, d;
  Real den = std::pow(sat2 - sat3, 3.0);
  Real pc2 = capillaryPressureUM(sat2);
  Real dpc2 = dCapillaryPressureUM(sat2);

  c = (3.0 * pc2 * (sat2 + sat3) - dpc2 * (sat2 - sat3) * (sat2 + 2.0 * sat3)) / den;
  d = (-2.0 * pc2 + dpc2 * (sat2 - sat3)) / den;

  return 2.0 * c + 6.0 * d * sat_liq;
}
