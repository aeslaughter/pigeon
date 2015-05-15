/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate saturation for multiphase flow */
/* in porous media.                                             */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "SaturationAux.h"

template<>
InputParameters validParams<SaturationAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredCoupledVar("liquid_saturation_variable", "The liquid saturation variable.");
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "The fluid state UserObject");
  return params;
}

SaturationAux::SaturationAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
    _liquid_saturation(coupledValue("liquid_saturation_variable"))
{}

Real SaturationAux::computeValue()
{
  return _fluid_state.saturation(_liquid_saturation[_qp]);
}
