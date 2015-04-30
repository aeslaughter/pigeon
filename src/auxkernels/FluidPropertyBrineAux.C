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

#include "FluidPropertyBrineAux.h"

template<>
InputParameters validParams<FluidPropertyBrineAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("fluid_property_uo", "Name of the User Object defining the fluid properties");
  params.addCoupledVar("pressure_variable", 1.e6,  "The pressure variable corresponding to the fluid phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  params.addCoupledVar("salt_mass_fraction_variable", 0.1, "The salt mass fraction variable.");
  params.addCoupledVar("saturation_variable", 1.0, "The saturation variable corresponding to the fluid phase.");
  MooseEnum fluid_property_enum("density viscosity");
  params.addRequiredParam<MooseEnum>("fluid_property_enum", fluid_property_enum, "The fluid property that this auxillary kernel is to calculate");
  return params;
}

FluidPropertyBrineAux::FluidPropertyBrineAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _fluid_property(getUserObject<FluidPropertiesBrine>("fluid_property_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _saturation(coupledValue("saturation_variable")),
    _salt_mass_fraction(coupledValue("saturation_variable")),
    fluid_property_enum(getParam<MooseEnum>("fluid_property_enum"))
{}

Real FluidPropertyBrineAux::computeValue()
{
  Real property;

  if (fluid_property_enum == "density") {
     property = _fluid_property.density(_pressure[_qp], _temperature[_qp], _salt_mass_fraction[_qp]);
  }

  return property;
}
