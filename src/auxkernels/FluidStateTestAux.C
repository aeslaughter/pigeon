/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidStateTestAux.h"

template<>
InputParameters validParams<FluidStateTestAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state");
  MooseEnum state_property_enum("density viscosity mass_fraction saturation pressure relperm ddensity_dp ddensity_ds ddensity_dx drelperm dmobility_dp dmobility_ds dmobility_dx");
  params.addRequiredParam<MooseEnum>("state_property_enum", state_property_enum, "The fluid property that this auxillary kernel is to calculate");
  params.addParam<unsigned int>("phase_index", 0, "The index of the phase this auxillary kernel acts on");
  params.addParam<unsigned int>("component_index", 0, "The index of the component this auxillary kernel acts on");
  return params;
}

FluidStateTestAux::FluidStateTestAux(const InputParameters & parameters) :
    AuxKernel(parameters),

    _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
    _state_property_enum(getParam<MooseEnum>("state_property_enum")),
    _phase_index(getParam<unsigned int>("phase_index")),
    _component_index(getParam<unsigned int>("component_index"))
{
}

Real
FluidStateTestAux::computeValue()
{
  Real property;
  unsigned int node_num = _current_node->id();

  if (_state_property_enum == "density")
      property = _fluid_state.getNodalProperty("density", node_num, _phase_index);

  else if (_state_property_enum == "viscosity")
    property = _fluid_state.getNodalProperty("viscosity", node_num, _phase_index);

  else if (_state_property_enum == "saturation")
    property = _fluid_state.getNodalProperty("saturation", node_num, _phase_index);

  else if (_state_property_enum == "pressure")
    property = _fluid_state.getNodalProperty("pressure", node_num, _phase_index);

  else if (_state_property_enum == "relperm")
    property = _fluid_state.getNodalProperty("relperm", node_num, _phase_index);

  else if (_state_property_enum == "mass_fraction")
    property = _fluid_state.getNodalProperty("mass_fraction", node_num, _phase_index, _component_index);

  else if (_state_property_enum == "ddensity_dp")
    property = _fluid_state.getNodalProperty("ddensity_dp", node_num, _phase_index);

  else if (_state_property_enum == "ddensity_ds")
    property = _fluid_state.getNodalProperty("ddensity_ds", node_num, _phase_index);

  else if (_state_property_enum == "ddensity_dx")
    property = _fluid_state.getNodalProperty("ddensity_dx", node_num, _phase_index, _component_index);

  else if (_state_property_enum == "dmobility_dp")
    property = _fluid_state.getNodalProperty("dmobility_dp", node_num, _phase_index);

  else if (_state_property_enum == "dmobility_ds")
    property = _fluid_state.getNodalProperty("dmobility_ds", node_num, _phase_index);

  else if (_state_property_enum == "dmobility_dx")
    property = _fluid_state.getNodalProperty("dmobility_dx", node_num, _phase_index, _component_index);

  return property;
}
