/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "CapillaryPressure.h"

template<>
InputParameters validParams<CapillaryPressure>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription("Capillary pressure base class.  Override capillaryPressure in your class");
  return params;
}

CapillaryPressure::CapillaryPressure(const InputParameters & parameters) :
  GeneralUserObject(parameters)
{}

void
CapillaryPressure::initialize()
{}

void
CapillaryPressure::execute()
{}

void
CapillaryPressure::finalize()
{}
