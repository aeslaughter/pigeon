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

/// Define perfectly mobile (unity) relative permeability and override relativePermLiq.
Real
RelativePermeabilityPerfectlyMobile::relativePermLiq(Real sat_liq) const
{
  
    return 1.0;

}

/// Define perfectly mobile (unity) relative permeability and override relativePermGas.
Real
RelativePermeabilityPerfectlyMobile::relativePermGas(Real sat_liq) const
{
  
    return 1.0;

}
