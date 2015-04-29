/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate fluid density using the given  */
/* Fluid Property UserObject for multiphase flow in porous      */
/* media.                                                       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDDENSITYAUX_H
#define FLUIDDENSITYAUX_H

#include "AuxKernel.h"
#include "FluidProperties.h"

class FluidDensityAux;

template<>
InputParameters validParams<FluidDensityAux>();

class FluidDensityAux : public AuxKernel
{
public:

  FluidDensityAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for fluid density (kg/m^3).
   */
  const FluidProperties & _fluid_property;

  /// Fluid pressure
  VariableValue & _pressure;
  /// Fluid temperature
  VariableValue & _temperature;
  /// Fluid phase saturation
  VariableValue & _saturation;
};

#endif //FLUIDDENSITYAUX_H
