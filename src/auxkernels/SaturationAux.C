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
  params.addRequiredCoupledVar("saturation_variable", "The primary saturation variable.");
  return params;
}

SaturationAux::SaturationAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _saturation(coupledValue("saturation_variable"))
{}

Real SaturationAux::computeValue()
{
  Real sat = 1.0 - _saturation[_qp];

  // Bound the saturation so that it is 0 <= sat <= 1 just in case
  if (sat < 0.0) sat = 0.0;
  if (sat > 1.0) sat = 1.0;

  return sat;
}
