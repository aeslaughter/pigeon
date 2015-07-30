/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "CapillaryPressureAux.h"

template<>
InputParameters validParams<CapillaryPressureAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("capillary_pressure_uo", "Name of the User Object defining the capillary pressure function");
  params.addRequiredCoupledVar("liquid_saturation_variable", "The liquid saturation variable.");
  MooseEnum property_enum("capillary_pressure dcapillary_pressure d2capillary_pressure", "capillary_pressure");
  params.addRequiredParam<MooseEnum>("property_enum", property_enum, "The capillary pressure that this auxillary kernel is to calculate");

  return params;
}

CapillaryPressureAux::CapillaryPressureAux(const InputParameters & parameters) :
    AuxKernel(parameters),

    _capillary_pressureUO(getUserObject<CapillaryPressure>("capillary_pressure_uo")),
    _liquid_saturation(coupledValue("liquid_saturation_variable")),
    _property_enum(getParam<MooseEnum>("property_enum"))
{}

Real
CapillaryPressureAux::computeValue()
{
  Real pc;

  if (_property_enum == "capillary_pressure")
      pc = _capillary_pressureUO.capillaryPressure(_liquid_saturation[_qp]);

  else if (_property_enum == "dcapillary_pressure")
      pc = _capillary_pressureUO.dCapillaryPressure(_liquid_saturation[_qp]);

  else if (_property_enum == "d2capillary_pressure")
      pc = _capillary_pressureUO.d2CapillaryPressure(_liquid_saturation[_qp]);

  else
    mooseError("Fluid property specified in " << _short_name << "is not valid");

  return pc;
}
