/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate fluid property using the given */
/* Fluid state UserObject for multiphase flow in porous         */
/* media.                                                       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDSTATEAUX_H
#define FLUIDSTATEAUX_H

#include "AuxKernel.h"
#include "FluidState.h"

class FluidStateAux;

template<>
InputParameters validParams<FluidStateAux>();

class FluidStateAux : public AuxKernel
{
public:

  FluidStateAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;

  /// Brine phase pressure
  VariableValue & _pressure;
  /// Brine phase temperature
  VariableValue & _temperature;
  /// Brine phase saturation
  VariableValue & _saturation;
  /// MooseEnum of fluid properties
  const MooseEnum & _state_property_enum;
};

#endif //FLUIDSTATEAUX_H
