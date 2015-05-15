/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Perfectly mobile relative permeability for multiphase flow   */
/* in porous media.                                             */
/*                                                              */
/* Relative permeability is unity for all saturations.          */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "RelativePermeabilityPerfectlyMobile.h"

template<>
InputParameters validParams<RelativePermeabilityPerfectlyMobile>()
{
  InputParameters params = validParams<RelativePermeability>();
  params.addClassDescription("Perfectly mobile form of relative permeability.");
  return params;
}

RelativePermeabilityPerfectlyMobile::RelativePermeabilityPerfectlyMobile(const std::string & name, InputParameters parameters) :
  RelativePermeability(name, parameters)
{
}

Real
RelativePermeabilityPerfectlyMobile::relativePermLiq(Real sat_liq) const
{

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityPerfectlyMobile: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  return 1.0;

}

Real
RelativePermeabilityPerfectlyMobile::relativePermGas(Real sat_liq) const
{

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("RelativePermeabilityPerfectlyMobile: Liquid saturation is outside the range 0 <= Sl <= 1\n");

  return 1.0;

}

Real
RelativePermeabilityPerfectlyMobile::dRelativePermLiquid(Real sat_liq) const
{
  return 0.0;
}

Real
RelativePermeabilityPerfectlyMobile::dRelativePermGas(Real sat_liq) const
{
  return 0.0;
}
