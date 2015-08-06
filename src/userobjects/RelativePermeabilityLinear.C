/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "RelativePermeabilityLinear.h"

template<>
InputParameters validParams<RelativePermeabilityLinear>()
{
  InputParameters params = validParams<RelativePermeability>();
  params.addClassDescription("Linear form of relative permeability.");
  return params;
}

RelativePermeabilityLinear::RelativePermeabilityLinear(const InputParameters & parameters) :
  RelativePermeability(parameters)
{
}

Real
RelativePermeabilityLinear::relativePermLiquid(Real sat_liq) const
{
  if (sat_liq < 0.0)
    sat_liq = 0.0;
  if (sat_liq > 1.0)
    sat_liq = 1.0;

  /// Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityLinear: Liquid saturation is outside the range 0 <= Sl <= 1");

  return sat_liq;

}

Real
RelativePermeabilityLinear::relativePermGas(Real sat_liq) const
{
  if (sat_liq < 0.0)
    sat_liq = 0.0;
  if (sat_liq > 1.0)
    sat_liq = 1.0;

  /// Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityLinear: Liquid saturation is outside the range 0 <= Sl <= 1");

  return 1.0 - sat_liq;

}

Real
RelativePermeabilityLinear::dRelativePermLiquid(Real sat_liq) const
{
  return 1.0;
}

Real
RelativePermeabilityLinear::dRelativePermGas(Real sat_liq) const
{
  return -1.0;
}
