/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidStateAux.h"

template<>
InputParameters validParams<FluidStateAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state");
  params.addCoupledVar("pressure_variable", 1.e6,  "The pressure variable corresponding to the phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  params.addCoupledVar("liquid_saturation_variable", 1.0, "The saturation variable corresponding to the liquid phase.");
  MooseEnum state_property_enum("density viscosity mass_fraction saturation pressure relperm");
  params.addRequiredParam<MooseEnum>("state_property_enum", state_property_enum, "The fluid property that this auxillary kernel is to calculate");
  params.addParam<unsigned int>("phase_index", 0, "The index of the phase this auxillary kernel acts on");
  params.addParam<unsigned int>("component_index", 0, "The index of the component this auxillary kernel acts on");
  return params;
}

FluidStateAux::FluidStateAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
    _pressure(coupledValue("pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _saturation(coupledValue("liquid_saturation_variable")),
    _state_property_enum(getParam<MooseEnum>("state_property_enum")),
    _phase_index(getParam<unsigned int>("phase_index")),
    _component_index(getParam<unsigned int>("component_index"))
{}

Real FluidStateAux::computeValue()
{
  Real temperature, property;

  // If the simulation is isothermal, take the temperature from the FluidState UserObject.
  if (_fluid_state.isIsothermal())
    temperature = _fluid_state.temperature();
  else
    temperature = _temperature[_qp];

  if (_state_property_enum == "density")
  {
    // If the saturation of this phase is zero, set density to zero.
    Real eps = 1.e-10; 
    if (_fluid_state.saturation(_saturation[_qp])[_phase_index] < eps)
      property = 0.;
    else
      property = _fluid_state.density(_pressure[_qp], temperature)[_phase_index];
  }
  if (_state_property_enum == "viscosity")
  {
    property = _fluid_state.viscosity(_pressure[_qp], temperature)[_phase_index];
  }
  if (_state_property_enum == "saturation")
  {
    property = _fluid_state.saturation(_saturation[_qp])[_phase_index];
  }
  if (_state_property_enum == "pressure")
  {
    property = _fluid_state.pressure(_pressure[_qp], _saturation[_qp]);
  }
  if (_state_property_enum == "relperm")
  {
    property = _fluid_state.relativePermeability(_saturation[_qp])[_phase_index];
  }
  if (_state_property_enum == "mass_fraction")
  {
    property = _fluid_state.massFractions(_pressure[_qp], temperature)[_phase_index][_component_index];
  }

  return property;
}
