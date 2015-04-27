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
  params.addRequiredCoupledVar("pressure_variable", "The pressure variable corresponding to the fluid phase.");
  params.addRequiredCoupledVar("temperature_variable", "The temperature variable.");
  params.addRequiredCoupledVar("saturation_variable", "The saturation variable corresponding to the fluid phase.");
  MooseEnum fluid_property_enum("density viscosity psat tsat");
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
     property = _fluid_property.fluidDensity(_pressure[_qp], _temperature[_qp]);
  }
  if (fluid_property_enum == "viscosity") {
     property = _fluid_property.fluidViscosity(_pressure[_qp], _temperature[_qp]);
  }
  if (fluid_property_enum == "psat") {
     property = _fluid_property.pSat(_temperature[_qp]);
  }
  if (fluid_property_enum == "tsat") {
     property = _fluid_property.tSat(_pressure[_qp]);
  }

  return property;
}
