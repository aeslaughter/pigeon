/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate nodal pressure of one phase    */
/* given the pressure of the other phase and the capillary      */
/* pressure defined in the Capillary Pressure UserObject.       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDPRESSUREAUX_H
#define FLUIDPRESSUREAUX_H

#include "AuxKernel.h"
#include "FluidState.h"

class FluidPressureAux;

template<>
InputParameters validParams<FluidPressureAux>();

class FluidPressureAux : public AuxKernel
{
public:

  FluidPressureAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for fluid state.
   */
  const FluidState & _fluid_state;

  /// Primary pressure variable at the nodes
  VariableValue & _primary_pressure;
  /// Liquid phase saturation at the nodes
  VariableValue & _liquid_saturation; 
};

#endif //FLUIDPRESSUREAUX_H
