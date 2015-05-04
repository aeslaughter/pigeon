/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate brine property using the given */
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
  params.addRequiredParam<UserObjectName>("brine_property_uo", "Name of the User Object defining the brine properties");
  params.addRequiredParam<UserObjectName>("water_property_uo", "Name of the User Object defining the water properties");
  params.addCoupledVar("pressure_variable", 1.e6,  "The pressure variable corresponding to the brine phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  params.addCoupledVar("salt_mass_fraction_variable", 0.1, "The salt mass fraction variable.");
  params.addCoupledVar("saturation_variable", 1.0, "The saturation variable corresponding to the brine phase.");
  MooseEnum brine_property_enum("density viscosity halite_density halite_solubility pSat");
  params.addRequiredParam<MooseEnum>("brine_property_enum", brine_property_enum, "The brine property that this auxillary kernel is to calculate");
  return params;
}

FluidPropertyBrineAux::FluidPropertyBrineAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _brine_property(getUserObject<FluidPropertiesBrine>("brine_property_uo")),
    _water_property(getUserObject<FluidPropertiesWater>("water_property_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _saturation(coupledValue("saturation_variable")),
    _salt_mass_fraction(coupledValue("salt_mass_fraction_variable")),
    brine_property_enum(getParam<MooseEnum>("brine_property_enum"))
{}

Real FluidPropertyBrineAux::computeValue()
{
  Real property;

  if (brine_property_enum == "density") {
    property = _brine_property.brineDensity(_pressure[_qp], _temperature[_qp], _salt_mass_fraction[_qp]);
  }
  if (brine_property_enum == "viscosity") {
    Real density = _water_property.density(_pressure[_qp], _temperature[_qp]);
    property = _brine_property.brineViscosity(_temperature[_qp], density,  _salt_mass_fraction[_qp]);
  }
  if (brine_property_enum == "halite_density") {
    property = _brine_property.haliteDensity(_pressure[_qp], _temperature[_qp]);
  }
  if (brine_property_enum == "halite_solubility") {
    property = _brine_property.haliteSolubility(_temperature[_qp]);
  }
  if (brine_property_enum == "pSat") {
    property = _brine_property.brinePSat(_temperature[_qp], _salt_mass_fraction[_qp]);
  }

  return property;
}
