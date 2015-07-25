/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidPropertyCO2Aux.h"

template<>
InputParameters validParams<FluidPropertyCO2Aux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("co2_property_uo", "Name of the User Object defining the co2 properties");
  params.addCoupledVar("pressure_variable", 1.e6,  "The pressure variable corresponding to the co2 phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  MooseEnum co2_property_enum("density viscosity dDensity_dP dDensity_dT partialDensity");
  params.addRequiredParam<MooseEnum>("co2_property_enum", co2_property_enum, "The co2 property that this auxillary kernel is to calculate");
  return params;
}

FluidPropertyCO2Aux::FluidPropertyCO2Aux(const InputParameters & parameters) :
    AuxKernel(parameters),

    _co2_property(getUserObject<FluidPropertiesCO2>("co2_property_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledNodalValue("temperature_variable")),
    co2_property_enum(getParam<MooseEnum>("co2_property_enum"))
{}

Real
FluidPropertyCO2Aux::computeValue()
{
  Real property;

  if (co2_property_enum == "density")
  {
    property = _co2_property.density(_pressure[_qp], _temperature[_qp]);
  }
  if (co2_property_enum == "viscosity")
  {
    property = _co2_property.viscosity(_pressure[_qp], _temperature[_qp]);
  }
  if (co2_property_enum == "dDensity_dP")
  {
    property = _co2_property.dDensity_dP(_pressure[_qp], _temperature[_qp]);
  }
  if (co2_property_enum == "dDensity_dT")
  {
    property = _co2_property.dDensity_dT(_pressure[_qp], _temperature[_qp]);
  }
  if (co2_property_enum == "partialDensity")
  {
    property = _co2_property.partialDensity(_temperature[_qp]);
  }

  return property;
}
