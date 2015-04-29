/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate fluid property using the given */
/* Fluid Property UserObject for multiphase flow in porous      */
/* media.                                                       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidPropertyAux.h"

template<>
InputParameters validParams<FluidPropertyAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("fluid_property_uo", "Name of the User Object defining the fluid properties");
  params.addCoupledVar("pressure_variable", 1.e6,  "The pressure variable corresponding to the fluid phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  params.addCoupledVar("saturation_variable", 1.0, "The saturation variable corresponding to the fluid phase.");
  MooseEnum fluid_property_enum("density viscosity");
  params.addRequiredParam<MooseEnum>("fluid_property_enum", fluid_property_enum, "The fluid property that this auxillary kernel is to calculate");
  return params;
}

FluidPropertyAux::FluidPropertyAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _fluid_property(getUserObject<FluidProperties>("fluid_property_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _saturation(coupledValue("saturation_variable")),
    fluid_property_enum(getParam<MooseEnum>("fluid_property_enum"))
{}

Real FluidPropertyAux::computeValue()
{
  Real property;

  if (fluid_property_enum == "density") {
     property = _fluid_property.density(_pressure[_qp], _temperature[_qp]);
  }
  if (fluid_property_enum == "viscosity") {
     Real density = _fluid_property.density(_pressure[_qp], _temperature[_qp]);
     property = _fluid_property.viscosity(_temperature[_qp], density);
  }

  return property;
}
