/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate capillary pressure             */
/* Pc = Pg - Pw for multiphase flow in porous media using the   */
/* capillary pressure relationship defined in the Capillary     */
/* Pressure UserObject.                                         */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
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

  CapillaryPressureAux(const std::string & name,
             InputParameters parameters);

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
};

#endif //CAPILLARYPRESSUREAUX_H
