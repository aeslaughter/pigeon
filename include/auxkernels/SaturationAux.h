/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef SATURATIONAUX_H
#define SATURATIONAUX_H

#include "AuxKernel.h"

class SaturationAux;

/**
 * SaturationAux takes in one phases saturation, and returns th
 * saturation of the other phase using the simple relationship
 * S_1 = 1 - S_2.
 *
 * As saturation is only required at the nodes, use a nodal
 * auxvariable.
 */

template<>
InputParameters validParams<SaturationAux>();

class SaturationAux : public AuxKernel
{
public:

  SaturationAux(const InputParameters & parameters);

protected:

  virtual Real computeValue();

private:
  
  VariableValue & _liquid_saturation;
};

#endif //SATURATIONAUX_H
