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
  MooseEnum state_property_enum("density viscosity mass_fraction saturation pressure relperm henry");
  params.addRequiredParam<MooseEnum>("state_property_enum", state_property_enum, "The fluid property that this auxillary kernel is to calculate");
  params.addParam<unsigned int>("phase_index", 0, "The index of the phase this auxillary kernel acts on");
  params.addParam<unsigned int>("component_index", 0, "The index of the component this auxillary kernel acts on");
  return params;
}

FluidStateAux::FluidStateAux(const std::string & name,
                       InputParameters parameters) :
    AuxKernel(name, parameters),

    _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
    _state_property_enum(getParam<MooseEnum>("state_property_enum")),
    _phase_index(getParam<unsigned int>("phase_index")),
    _component_index(getParam<unsigned int>("component_index"))
{
}

Real
FluidStateAux::computeValue()
{
  Real property;
  unsigned int node_num = _current_node->id();

  if (_state_property_enum == "density")
  {
      property = _fluid_state.getDensity(node_num, _phase_index);
  }

  if (_state_property_enum == "viscosity")
  {
    property = _fluid_state.getViscosity(node_num, _phase_index);
  }

  if (_state_property_enum == "saturation")
  {
    property = _fluid_state.getSaturation(node_num, _phase_index);
  }
  if (_state_property_enum == "pressure")
  {
    property = _fluid_state.getPressure(node_num, _phase_index);
  }
  if (_state_property_enum == "relperm")
  {
    property = _fluid_state.getRelativePermeability(node_num, _phase_index);
  }

  if (_state_property_enum == "mass_fraction")
  {
    property = _fluid_state.getMassFraction(node_num, _phase_index, _component_index);
  }
    /*
  if (_state_property_enum == "henry")
  {
    property = _fluid_state.henry(temperature);
  }
*/

  return property;
}
