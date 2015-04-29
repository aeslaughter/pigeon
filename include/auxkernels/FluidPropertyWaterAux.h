/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate fluid property using the given */
/* Fluid Property UserObject for multiphase flow in porous      */
/* media.                                                       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef FLUIDPROPERTYWATERAUX_H
#define FLUIDPROPERTYWATERAUX_H

#include "AuxKernel.h"
#include "FluidPropertiesWater.h"

class FluidPropertyWaterAux;

template<>
InputParameters validParams<FluidPropertyWaterAux>();

class FluidPropertyWaterAux : public AuxKernel
{
public:

  FluidPropertyWaterAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for fluid property.
   */
  const FluidPropertiesWater & _fluid_property;

  /// Water phase pressure
  VariableValue & _pressure;
  /// Water phase temperature
  VariableValue & _temperature;
  /// Water phase saturation
  VariableValue & _saturation;
  /// MooseEnum of fluid properties
  const MooseEnum & fluid_property_enum;
};

#endif //FLUIDPROPERTYWATERAUX_H
