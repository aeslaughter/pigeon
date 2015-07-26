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

/**
 * Derivative of mass of component k in fluid phase alpha wrt time
 * In this formulation, all mass is lumped to the nodes of the element
 */
class ComponentMassTimeDerivative : public TimeKernel
{
public:
  ComponentMassTimeDerivative(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  /// Member reference to the porosity material property
  const MaterialProperty<Real> & _porosity;


private:
  VariableValue & _fluid_density;
  VariableValue & _fluid_density_old;
  VariableValue & _fluid_saturation;
  VariableValue & _fluid_saturation_old;
  VariableValue & _component_mass_fraction;
  VariableValue & _component_mass_fraction_old;
  VariableValue & _fluid_pressure;
  VariableValue & _fluid_pressure_old;
  VariableValue & _temperature;

  /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;

  /**
   * The sign of the derivative of the saturation variable that this kernel
   * acts on wrt the liquid phase saturation
   */
  Real _dsaturation;
  /// Index of the fluid phase that this kernel acts on
  unsigned int _phase_index;
  /// Primary variable type (used in Jacobian calculations)
  std::string _primary_variable_type;
};

#endif //COMPONENTMASSTIMEDERIVATIVE_H
