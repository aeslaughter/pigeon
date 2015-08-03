/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidPropertyWaterAux.h"

template<>
InputParameters validParams<FluidPropertyWaterAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("fluid_property_uo", "Name of the User Object defining the fluid properties");
  params.addCoupledVar("pressure_variable", 1.e6,  "The pressure variable corresponding to the fluid phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  params.addCoupledVar("saturation_variable", 1.0, "The saturation variable corresponding to the fluid phase.");
  MooseEnum fluid_property_enum("density viscosity tsat psat b23t b23p region1 region2 dDensity_dP");
  params.addRequiredParam<MooseEnum>("fluid_property_enum", fluid_property_enum, "The fluid property that this auxillary kernel is to calculate");
  return params;
}

FluidPropertyWaterAux::FluidPropertyWaterAux(const InputParameters & parameters) :
    AuxKernel(parameters),

    _fluid_property(getUserObject<FluidPropertiesWater>("fluid_property_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _saturation(coupledValue("saturation_variable")),
    fluid_property_enum(getParam<MooseEnum>("fluid_property_enum"))
{}

Real
FluidPropertyWaterAux::computeValue()
{
  Real property;

  if (fluid_property_enum == "density")
    property = _fluid_property.density(_pressure[_qp], _temperature[_qp]);

  else if (fluid_property_enum == "viscosity")
  {
    Real density = _fluid_property.density(_pressure[_qp], _temperature[_qp]);
    property = _fluid_property.viscosity(_pressure[_qp], _temperature[_qp], density);
  }

  else if (fluid_property_enum == "tsat")
    property = _fluid_property.tSat(_pressure[_qp]);

  else if (fluid_property_enum == "psat")
    property = _fluid_property.pSat(_temperature[_qp]);

  else if (fluid_property_enum == "b23t")
    property = _fluid_property.b23t(_pressure[_qp]);

  else if (fluid_property_enum == "b23p")
    property = _fluid_property.b23p(_temperature[_qp]);

  else if (fluid_property_enum == "region1")
    property = _fluid_property.densityRegion1(_pressure[_qp], _temperature[_qp]);

  else if (fluid_property_enum == "region2")
    property = _fluid_property.densityRegion2(_pressure[_qp], _temperature[_qp]);

  else if (fluid_property_enum == "dDensity_dP")
    property = _fluid_property.dDensity_dP(_pressure[_qp], _temperature[_qp]);

  else
    mooseError("Fluid property specified in " << _short_name << "is not valid");

  return property;
}
