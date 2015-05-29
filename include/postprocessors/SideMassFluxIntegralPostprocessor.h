/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef SIDEMASSFLUXINTEGRALPOSTPROCESSOR_H
#define SIDEMASSFLUXINTEGRALPOSTPROCESSOR_H

#include "SideIntegralVariablePostprocessor.h"

class SideMassFluxIntegralPostprocessor;

template<>
InputParameters validParams<SideMassFluxIntegralPostprocessor>();

class SideMassFluxIntegralPostprocessor : public SideIntegralVariablePostprocessor
{
public:
  SideMassFluxIntegralPostprocessor(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpIntegral();

  MaterialProperty<RealTensorValue> & _permeability;
  MaterialProperty<RealVectorValue> & _gravity;

  VariableValue & _fluid_density;
  VariableValue & _fluid_viscosity;
  VariableValue & _mass_fraction;
  VariableValue & _relative_permeability;

};

#endif // SIDEMASSFLUXINTEGRALPOSTPROCESSOR_H
