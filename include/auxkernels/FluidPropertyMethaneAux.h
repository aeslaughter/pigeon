/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef FLUIDPROPERTYMETHANEAUX_H
#define FLUIDPROPERTYMETHANEAUX_H

#include "AuxKernel.h"
#include "FluidPropertiesMethane.h"

class FluidPropertyMethaneAux;

template<>
InputParameters validParams<FluidPropertyMethaneAux>();

class FluidPropertyMethaneAux : public AuxKernel
{
public:

  FluidPropertyMethaneAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  /**
   * This is the member reference that will hold the User Object
   * value for the CH4 property User Object.
   */
  const FluidPropertiesMethane & _ch4_property;

  /// CH4 gas pressure
  VariableValue & _pressure;
  /// CH4 gas temperature
  VariableValue & _temperature;
  /// MooseEnum of fluid properties
  const MooseEnum & ch4_property_enum;
};

#endif //FLUIDPROPERTYMETHANEAUX_H
