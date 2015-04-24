/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Constant capillary pressure for multiphase flow in porous    */
/*  media.                                                      */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "CapillaryPressureConstant.h"

template<>
InputParameters validParams<CapillaryPressureConstant>()
{
  InputParameters params = validParams<CapillaryPressure>();
  params.addRequiredParam<Real>("cp",  "Constant value of capillary pressure");
  params.addClassDescription("Constant capillary pressure.");
  return params;
}

CapillaryPressureConstant::CapillaryPressureConstant(const std::string & name, InputParameters parameters) :
  CapillaryPressure(name, parameters),
  _cp(getParam<Real>("cp"))
{
}

/// Override capillary_pressure.
Real
CapillaryPressureConstant::capillaryPressure(Real sat_liq) const
{
  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0)
    mooseError("CapillaryPressureConstant: Liquid saturation is negative\n");
  if (sat_liq > 1.0)
    mooseError("CapillaryPressureConstant: Liquid saturation is greater than 1.0\n");

  return _cp;
}
