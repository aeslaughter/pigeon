/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "CapillaryPressureLinear.h"

template<>
InputParameters validParams<CapillaryPressureLinear>()
{
  InputParameters params = validParams<CapillaryPressure>();
  params.addRequiredParam<Real>("cp_max",  "Maximum capillary pressure");
  params.addClassDescription("Linear capillary pressure.");
  return params;
}

CapillaryPressureLinear::CapillaryPressureLinear(const InputParameters & parameters) :
  CapillaryPressure(parameters),
  _cp_max(getParam<Real>("cp_max"))
{
}

Real
CapillaryPressureLinear::capillaryPressure(Real sat_liq) const
{
  Real pc = _cp_max * (sat_liq - 1.0);

  // Check whether liquid saturation is [0,1] - if not, print error message.
  if (sat_liq < 0.0 || sat_liq > 1.0)
    mooseError("CapillaryPressureLinear: Liquid saturation is outside the range 0 <= Sl <= 1");

  return pc;
}

Real
CapillaryPressureLinear::dCapillaryPressure(Real sat_liq) const
{
  return _cp_max;
}


Real
CapillaryPressureLinear::d2CapillaryPressure(Real sat_liq) const
{
  return 0.0;
}
