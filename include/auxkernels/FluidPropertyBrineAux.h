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

#ifndef FLUIDPROPERTYBRINEAUX_H
#define FLUIDPROPERTYBRINEAUX_H

#include "AuxKernel.h"
#include "FluidPropertiesBrine.h"

class FluidPropertyBrineAux;

template<>
InputParameters validParams<FluidPropertyBrineAux>();

class FluidPropertyBrineAux : public AuxKernel
{
public:

  FluidPropertyBrineAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for the brine property User Object.
   */
  const FluidPropertiesBrine & _brine_property;

  /**
   * This is the member reference that will hold the User Object
   * value for the water property User Object.
   */
  const FluidPropertiesWater & _water_property;

  /// Brine phase pressure
  VariableValue & _pressure;
  /// Brine phase temperature
  VariableValue & _temperature;
  /// Brine phase saturation
  VariableValue & _saturation;
  /// Salt mass fraction in the brine 
  VariableValue & _salt_mass_fraction;
  /// MooseEnum of fluid properties
  const MooseEnum & brine_property_enum;
};

#endif //FLUIDPROPERTYBRINEAUX_H
