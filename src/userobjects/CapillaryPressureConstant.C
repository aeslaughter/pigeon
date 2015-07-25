/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
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

CapillaryPressureConstant::CapillaryPressureConstant(const InputParameters & parameters) :
  CapillaryPressure(parameters),
  _cp(getParam<Real>("cp"))
{
}

Real
CapillaryPressureConstant::capillaryPressure(Real sat_liq) const
{
  if (sat_liq < 0.0)
    sat_liq = 0.0;
  if (sat_liq > 1.0)
    sat_liq = 1.0;

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("CapillaryPressureConstant: Liquid saturation is outside the range 0 <= Sl <= 1");

  return _cp;
}

Real
CapillaryPressureConstant::dCapillaryPressure(Real sat_liq) const
{
  return 0.0;
}


Real
CapillaryPressureConstant::d2CapillaryPressure(Real sat_liq) const
{
  return 0.0;
}
