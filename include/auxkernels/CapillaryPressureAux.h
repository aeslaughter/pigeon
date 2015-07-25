/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef CAPILLARYPRESSUREAUX_H
#define CAPILLARYPRESSUREAUX_H

#include "AuxKernel.h"
#include "CapillaryPressure.h"

class CapillaryPressureAux;

template<>
InputParameters validParams<CapillaryPressureAux>();

class CapillaryPressureAux : public AuxKernel
{
public:

  CapillaryPressureAux(const InputParameters & parameters);

protected:

  virtual Real computeValue();

private:

  /**
    * This is the member reference that will hold the User Object
    * value for capillary pressure (Pa).
    */
  const CapillaryPressure & _capillary_pressureUO;

  /// Liquid phase saturation
  VariableValue & _liquid_saturation;

  /// MooseEnum of capillary pressure functions
  const MooseEnum & _property_enum;
};

#endif //CAPILLARYPRESSUREAUX_H
