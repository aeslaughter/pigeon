/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidPressureAux.h"

template<>
InputParameters validParams<FluidPressureAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredCoupledVar("primary_pressure_variable", "The primary nonlinear pressure variable.");
  params.addRequiredCoupledVar("liquid_saturation_variable", "The liquid saturation variable.");
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "The fluid state UserObject");
  return params;
}

FluidPressureAux::FluidPressureAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
    _primary_pressure(coupledValue("primary_pressure_variable")),
    _liquid_saturation(coupledValue("liquid_saturation_variable"))
{}

Real
FluidPressureAux::computeValue()
{
  return _fluid_state.pressure(_primary_pressure[_qp], _liquid_saturation[_qp])[0];
}
