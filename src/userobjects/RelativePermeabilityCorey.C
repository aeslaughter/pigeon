/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "RelativePermeabilityCorey.h"

template<>
InputParameters validParams<RelativePermeabilityCorey>()
{
  InputParameters params = validParams<RelativePermeability>();
  params.addRequiredRangeCheckedParam<Real>("sat_lr", "sat_lr >= 0 & sat_lr < 1", "Liquid residual saturation.  Must be between 0 and 1.");
  params.addRequiredRangeCheckedParam<Real>("sat_gr", "sat_gr >= 0 & sat_gr < 1", "Gas residual saturation.  Must be between 0 and 1.");
  params.addClassDescription("Corey form of relative permeability.");
  return params;
}

RelativePermeabilityCorey::RelativePermeabilityCorey(const InputParameters & parameters) :
  RelativePermeability(parameters),
  _sat_lr(getParam<Real>("sat_lr")),
  _sat_gr(getParam<Real>("sat_gr"))
{
}

Real
RelativePermeabilityCorey::relativePermLiquid(Real sat_liq) const
{
  /// Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityCorey: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  Real sat_eff = (sat_liq - _sat_lr)/(1.0 - _sat_lr - _sat_gr);
  Real krel = std::pow(sat_eff, 4.0);

  /// Bound just in case
  if (krel < 0) { krel = 0;}
  if (krel > 1) { krel = 1;}

  return krel;
}

Real
RelativePermeabilityCorey::relativePermGas(Real sat_liq) const
{
  /// Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityCorey: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  Real sat_eff = (sat_liq - _sat_lr)/(1.0 - _sat_lr - _sat_gr);
  Real krel = std::pow(1.0 - sat_eff, 2.0) * (1.0 - std::pow(sat_eff, 2.0));

  /// Bound relative permeability in regions where liquid saturation is less than or greater than
  /// gas and liquid residual values
  if (krel < 0) { krel = 0;}
  if (krel > 1) { krel = 1;}

  return krel;
}

Real
RelativePermeabilityCorey::dRelativePermLiquid(Real sat_liq) const
{
  Real sat_eff = (sat_liq - _sat_lr)/(1.0 - _sat_lr - _sat_gr);
  Real dkrel;

  /// If Sl > 1 - Sgr, or Sl < Slr, krl is horizontal and hence dkrl = 0
  if (sat_liq > _sat_lr && sat_liq < 1.0 - _sat_gr)
    dkrel = 4.0 * std::pow(sat_eff, 3.0) / (1.0 - _sat_lr - _sat_gr);
  else
    dkrel = 0.0;

  return dkrel;
}

Real
RelativePermeabilityCorey::dRelativePermGas(Real sat_liq) const
{
  Real sat_eff = (sat_liq - _sat_lr)/(1.0 - _sat_lr - _sat_gr);
  Real dkrel;

  /// If Sl > 1 - Sgr, or Sl < Slr, krg is horizontal and hence dkrg = 0
  if (sat_liq > _sat_lr && sat_liq < 1.0 - _sat_gr)
    dkrel = (-2.0 * (1.0 + 2.0 * sat_eff) * std::pow(1.0 - sat_eff, 2.0)) / (1.0 - _sat_lr - _sat_gr);
  else
    dkrel = 0.0;

  return dkrel;
}
