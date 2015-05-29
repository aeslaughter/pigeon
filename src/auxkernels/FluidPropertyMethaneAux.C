/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidPropertyMethaneAux.h"

template<>
InputParameters validParams<FluidPropertyMethaneAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("ch4_property_uo", "Name of the User Object defining the methane properties");
  params.addCoupledVar("pressure_variable", 1.e6,  "The pressure variable corresponding to the mehane gas phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  MooseEnum ch4_property_enum("density viscosity");
  params.addRequiredParam<MooseEnum>("ch4_property_enum", ch4_property_enum, "The methane property that this auxillary kernel is to calculate");
  return params;
}

FluidPropertyMethaneAux::FluidPropertyMethaneAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _ch4_property(getUserObject<FluidPropertiesMethane>("ch4_property_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    ch4_property_enum(getParam<MooseEnum>("ch4_property_enum"))
{}

Real
FluidPropertyMethaneAux::computeValue()
{
  Real property;

  if (ch4_property_enum == "density")
  {
    property = _ch4_property.density(_pressure[_qp], _temperature[_qp]);
  }
  if (ch4_property_enum == "viscosity")
  {
    property = _ch4_property.viscosity(_pressure[_qp], _temperature[_qp]);
  }

  return property;
}
