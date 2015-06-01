/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef COMPONENTMASSPOSTPROCESSOR_H
#define COMPONENTMASSPOSTPROCESSOR_H

#include "ElementIntegralPostprocessor.h"

class ComponentMassPostprocessor;

template<>
InputParameters validParams<ComponentMassPostprocessor>();

class ComponentMassPostprocessor : public ElementIntegralPostprocessor
{
public:
  ComponentMassPostprocessor(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpIntegral();

  MaterialProperty<Real> & _porosity;
  MaterialProperty<std::vector<Real> > & _phase_mass;

  unsigned int _phase_index;
  VariableValue & _fluid_density;
  VariableValue & _fluid_saturation;

};

#endif // COMPONENTMASSPOSTPROCESSOR_H
