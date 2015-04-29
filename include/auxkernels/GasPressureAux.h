/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Auxillary kernel to calculate pressure of the gas phase      */
/* for multiphase flow in porous media using the capillary      */
/* pressure defined in the Capillary Pressure UserObject.       */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#ifndef GASPRESSUREAUX_H
#define GASPRESSUREAUX_H

#include "AuxKernel.h"

class GasPressureAux;

template<>
InputParameters validParams<GasPressureAux>();

class GasPressureAux : public AuxKernel
{
public:

  GasPressureAux(const std::string & name,
             InputParameters parameters);

protected:

  virtual Real computeValue();

private:

  VariableValue & _liquid_pressure;
  VariableValue & _capillary_pressure;
};

#endif //GASPRESSUREAUX_H
