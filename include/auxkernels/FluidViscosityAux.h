/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate fluid viscosity using the given*/
/* Fluid Property UserObject for multiphase flow in porous      */
/* media.                                                       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDVISCOSITYAUX_H
#define FLUIDVISCOSITYAUX_H

#include "AuxKernel.h"
#include "FluidProperties.h"

class FluidViscosityAux;

template<>
InputParameters validParams<FluidViscosityAux>();

class FluidViscosityAux : public AuxKernel
{
public:

  FluidViscosityAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for fluid viscosity (Pa.s).
   */
  const FluidProperties & _fluid_property;

  /// Fluid phase pressure
  VariableValue & _pressure;
  /// Fluid phase temperature
  VariableValue & _temperature;
};

#endif //FLUIDVISCOSITYAUX_H
