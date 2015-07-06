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
  InputParameters params = validParams<NodalUserObject>();
  params.addClassDescription("Fluid state base class.  Override properties in your class");
  return params;
}

FluidState::FluidState(const std::string & name, InputParameters parameters) :
  NodalUserObject(name, parameters)
{
}

void
FluidState::initialize()
{}

void
FluidState::execute()
{}

void
FluidState::finalize()
{}

void
FluidState::threadJoin(const UserObject & uo)
{}
