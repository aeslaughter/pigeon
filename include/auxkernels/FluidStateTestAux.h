/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDSTATETESTAUX_H
#define FLUIDSTATETESTAUX_H

#include "AuxKernel.h"
#include "FluidState.h"

class FluidStateTestAux;

template<>
InputParameters validParams<FluidStateTestAux>();

class FluidStateTestAux : public AuxKernel
{
public:

  FluidStateTestAux(const InputParameters & parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;

  /// MooseEnum of fluid properties
  const MooseEnum & _state_property_enum;
  /// Phase index
  unsigned int _phase_index;
  /// Component index
  unsigned int _component_index;
};

#endif //FLUIDSTATETESTAUX_H
