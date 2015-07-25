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
  ComponentMassPostprocessor(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral();

  const MaterialProperty<Real> & _porosity;
//  MaterialProperty<std::vector<Real> > & _phase_mass;

  std::vector<VariableValue *> _fluid_density;
  std::vector<VariableValue *> _fluid_saturation;
  std::vector<VariableValue *> _component_mass_fraction;

};

#endif // COMPONENTMASSPOSTPROCESSOR_H
