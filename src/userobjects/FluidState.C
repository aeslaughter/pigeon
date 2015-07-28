/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidState.h"

template<>
InputParameters validParams<FluidState>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription("Fluid state base class.  Override properties in your class");
  return params;
}

FluidState::FluidState(const InputParameters & parameters) :
  GeneralUserObject(parameters),
  Coupleable(parameters, false),
  _mesh(_subproblem.mesh()),
  _qp(0)

{
}


void
FluidState::initialize()
{
}

void
FluidState::execute()
{}

void
FluidState::finalize()
{}

Real
FluidState::getNodalProperty(std::string property, unsigned int nodeid, unsigned int phase_index, unsigned int component_index) const
{
  if (phase_index >= numPhases())
    mooseError("Phase index " << phase_index << " out of range in FluidStateSinglePhase::getNodalProperty");
  FluidStateProperties fsp;
  Real value=0;

  std::map<int, FluidStateProperties>::const_iterator node_it = _nodal_properties.find(nodeid);

  if (node_it != _nodal_properties.end())
    fsp = node_it->second;
  else
    return value; // Return zero if the node is out of range initially
  //  mooseError("Node id "<< nodeid << " out of range in FluidStateTwoPhase::getNodalProperty");

  // Now access the property and phase index
  if (property == "pressure")
    value = fsp.pressure[phase_index];
  else if (property == "saturation")
    value = fsp.saturation[phase_index];
  else if (property == "density")
    value = fsp.density[phase_index];
  else if (property == "viscosity")
    value = fsp.viscosity[phase_index];
  else if (property == "relperm")
    value = fsp.relperm[phase_index];
  else if (property == "mass_fraction")
    value = fsp.mass_fraction[component_index][phase_index];
  else if (property == "mobility")
    value = fsp.mobility[phase_index];
  else if (property == "ddensity_dp")
    value = fsp.ddensity_dp[phase_index];
  else if (property == "ddensity_ds")
    value = fsp.ddensity_ds[phase_index];
  else if (property == "ddensity_dx")
    value = fsp.ddensity_dx[component_index][phase_index];
  else if (property == "drelperm")
    value = fsp.drelperm[phase_index];
  else if (property == "dmobility_dp")
    value = fsp.dmobility_dp[phase_index];
  else if (property == "dmobility_ds")
    value = fsp.dmobility_ds[phase_index];
  else if (property == "dmobility_dx")
    value = fsp.dmobility_dx[component_index][phase_index];
  else
    mooseError("Property " << property << " in FluidStateSinglePhase::getNodalProperty is not one of the members of the FluidStateProperties class. Check spelling of property.");

  return value;
}
