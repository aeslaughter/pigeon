/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDPRESSUREAUX_H
#define FLUIDPRESSUREAUX_H

#include "AuxKernel.h"
#include "CapillaryPressure.h"

class FluidPressureAux;

template<>
InputParameters validParams<FluidPressureAux>();

class FluidPressureAux : public AuxKernel
{
public:

  FluidPressureAux(const InputParameters & parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for the capillary pressure.
   */
  const CapillaryPressure & _capillary_pressure;

  /// Primary pressure variable at the nodes
  VariableValue & _primary_pressure;
  /// Liquid phase saturation at the nodes
  VariableValue & _liquid_saturation;
};

#endif //FLUIDPRESSUREAUX_H
