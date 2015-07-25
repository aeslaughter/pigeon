/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "RelativePermeabilityVanGenuchten.h"

template<>
InputParameters validParams<RelativePermeabilityVanGenuchten>()
{
  InputParameters params = validParams<RelativePermeability>();
  params.addRequiredRangeCheckedParam<Real>("sat_lr", "sat_lr >= 0 & sat_lr <= 1", "Liquid residual saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("sat_ls", "sat_ls >= 0 & sat_ls <= 1", "Liquid fully saturated saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("sat_gr", "sat_gr >= 0 & sat_gr <= 1", "Gas residual saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("m", "m > 0 & m < 1", "van Genuchten exponent m.");
  params.addClassDescription("van Genuchten form of relative permeability.");
  return params;
}

RelativePermeabilityVanGenuchten::RelativePermeabilityVanGenuchten(const std::string & name, InputParameters parameters) :
  RelativePermeability(name, parameters),
  _sat_lr(getParam<Real>("sat_lr")),
  _sat_ls(getParam<Real>("sat_ls")),
  _sat_gr(getParam<Real>("sat_gr")),
  _m(getParam<Real>("m"))
{
}

Real
RelativePermeabilityVanGenuchten::relativePermLiquid(Real sat_liq) const
{
  if (sat_liq < 0.0 )
    sat_liq = 0.0;
  if (sat_liq > 1.0)
    sat_liq = 1.0;

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityVanGenuchten: Liquid saturation is outside the range 0 <= Sl <= 1\n");


  if (sat_liq >= _sat_ls) {
    return 1.0;
  }

  if (sat_liq <= _sat_lr) {
    return 0.0;
  }

  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);

  // Bound s_internal to [0,1]
  if (sat_eff < 0.) { sat_eff = 0.;}
  if (sat_eff > 1.) { sat_eff = 1.;}

  Real krel = std::pow(sat_eff, 0.5)*std::pow(1.0 - std::pow(1.0 - std::pow(sat_eff, 1.0/_m), _m), 2.0);

  // Bound just in case
  if (krel < 0) { krel = 0;}
  if (krel > 1) { krel = 1;}
  return krel;
}

Real
RelativePermeabilityVanGenuchten::relativePermGas(Real sat_liq) const
{
  Real krel;

  if (sat_liq < 0.0 )
    sat_liq = 0.0;
  if (sat_liq > 1.0)
    sat_liq = 1.0;

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityVanGenuchten: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  if (sat_liq >= _sat_ls)
    return 0.0;


  if (sat_liq <= _sat_lr)
    return 1.0;


  if (_sat_gr > 0.0) {

  Real sat_eff = (sat_liq - _sat_lr)/(1.0 - _sat_lr - _sat_gr);

  // Bound s_internal to [0,1]
  if (sat_eff < 0.) { sat_eff = 0.;}
  if (sat_eff > 1.) { sat_eff = 1.;}

  krel = std::pow(1.0 - sat_eff, 2.0) * (1.0 - std::pow(sat_eff, 2.0));

  // Bound just in case
  if (krel < 0) { krel = 0;}
  if (krel > 1) { krel = 1;}
  }

  else
    krel = 1.0 - relativePermLiquid(sat_liq);


  return krel;
}

Real
RelativePermeabilityVanGenuchten::dRelativePermLiquid(Real sat_liq) const
{
  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityVanGenuchten: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  if (sat_liq >= _sat_ls)
    return 0.0;

  if (sat_liq <= _sat_lr)
    return 0.0;

  Real sat_eff = (sat_liq - _sat_lr)/(_sat_ls - _sat_lr);
  Real a = 1.0 - std::pow(sat_eff, 1.0 / _m);
  Real a2 = 1.0 - std::pow(a, _m);

  Real dkrel = (0.5 * std::pow(sat_eff, -0.5) * a2 * a2 + 2.0 * std::pow(sat_eff, 1.0/_m - 0.5) * std::pow(a, _m - 1.0) * a2) / (sat_liq - _sat_lr);

  return dkrel;
}

Real
RelativePermeabilityVanGenuchten::dRelativePermGas(Real sat_liq) const
{
  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityVanGenuchten: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  Real dkrel;

  if (_sat_gr > 0.0)
  {
    Real sat_eff = (sat_liq - _sat_lr)/(1.0 - _sat_lr - _sat_gr);
    dkrel = (-2.0 * (1.0 + 2.0 * sat_eff) * std::pow(1.0 - sat_eff, 2.0)) / (1.0 - _sat_lr - _sat_gr);
  }
  else
    dkrel = - dRelativePermLiquid(sat_liq);

  return dkrel;
}
