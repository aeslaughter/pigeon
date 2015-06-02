/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
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

  /// Phase pressure
  VariableValue & _pressure;
  /// Temperature
  VariableValue & _temperature;
  /// Phase saturation
  VariableValue & _saturation;
  /// MooseEnum of fluid properties
  const MooseEnum & _state_property_enum;
  /// Phase index
  unsigned int _phase_index;
  /// Component index
  unsigned int _component_index;
};

#endif //FLUIDSTATEAUX_H
