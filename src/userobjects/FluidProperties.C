/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidProperties.h"

template<>
InputParameters validParams<FluidProperties>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription("Fluid properties base class.  Override properties in your class");
  return params;
}

FluidProperties::FluidProperties(const std::string & name, InputParameters parameters) :
  GeneralUserObject(name, parameters)
{}

void
FluidProperties::initialize()
{}

void
FluidProperties::execute()
{}

void
FluidProperties::finalize()
{}
