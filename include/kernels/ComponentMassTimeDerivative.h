/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef COMPONENTMASSTIMEDERIVATIVE_H
#define COMPONENTMASSTIMEDERIVATIVE_H

#include "TimeKernel.h"
#include "FluidState.h"

class ComponentMassTimeDerivative;

template<>
InputParameters validParams<ComponentMassTimeDerivative>();

class ComponentMassTimeDerivative : public TimeKernel
{
public:
  ComponentMassTimeDerivative(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // Member reference to the porosity material property
  const MaterialProperty<Real> & _porosity;


private:
  VariableValue & _fluid_density;
  VariableValue & _fluid_density_old;
  VariableValue & _fluid_saturation;
  VariableValue & _fluid_saturation_old;
  VariableValue & _component_mass_fraction;
  VariableValue & _component_mass_fraction_old;
  VariableValue & _fluid_pressure;
  const MooseEnum & _primary_variable_type;
  VariableValue & _temperature;

  /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;
  unsigned int _phase_index;
};

#endif //COMPONENTMASSTIMEDERIVATIVE_H
