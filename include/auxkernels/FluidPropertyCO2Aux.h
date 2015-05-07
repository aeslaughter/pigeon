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

#ifndef FLUIDPROPERTYCO2AUX_H
#define FLUIDPROPERTYCO2AUX_H

#include "AuxKernel.h"
#include "FluidPropertiesCO2.h"

class FluidPropertyCO2Aux;

template<>
InputParameters validParams<FluidPropertyCO2Aux>();

class FluidPropertyCO2Aux : public AuxKernel
{
public:

  FluidPropertyCO2Aux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for the co2 property User Object.
   */
  const FluidPropertiesCO2 & _co2_property;

  /// CO2 phase pressure
  VariableValue & _pressure;
  /// CO2 phase temperature
  VariableValue & _temperature;
  /// MooseEnum of fluid properties
  const MooseEnum & co2_property_enum;
};

#endif //FLUIDPROPERTYCO2AUX_H
