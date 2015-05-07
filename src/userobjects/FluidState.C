/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid state base clase for multiphase flow in                */
/* porous media                                                 */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidState.h"

template<>
InputParameters validParams<FluidState>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription("Fluid state base class.  Override properties in your class");
  return params;
}

FluidState::FluidState(const std::string & name, InputParameters parameters) :
  GeneralUserObject(name, parameters)
{}

void
FluidState::initialize()
{}

void
FluidState::execute()
{}

void
FluidState::finalize()
{}
