/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "SaturationAux.h"

template<>
InputParameters validParams<SaturationAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredCoupledVar("liquid_saturation_variable", "The liquid saturation variable.");
  return params;
}

SaturationAux::SaturationAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _liquid_saturation(coupledValue("liquid_saturation_variable"))
{}

Real
SaturationAux::computeValue()
{
  return 1.0 - _liquid_saturation[_qp];
}
