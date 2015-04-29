/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Relative permeability base clase for multiphase flow in      */
/* porous media                                                 */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "RelativePermeability.h"

template<>
InputParameters validParams<RelativePermeability>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addClassDescription("Relative permeability base class.  Override relativePermLiq and relativePermGas in your class");
  return params;
}

RelativePermeability::RelativePermeability(const std::string & name, InputParameters parameters) :
  GeneralUserObject(name, parameters)
{}

void
RelativePermeability::initialize()
{}

void
RelativePermeability::execute()
{}

void
RelativePermeability::finalize()
{}
