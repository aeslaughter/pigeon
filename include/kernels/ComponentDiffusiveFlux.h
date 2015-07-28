/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef COMPONENTDIFFUSIVEFLUX_H
#define COMPONENTDIFFUSIVEFLUX_H

#include "Kernel.h"
#include "FluidState.h"

class ComponentDiffusiveFlux;

template<>
InputParameters validParams<ComponentDiffusiveFlux>();

/**
 * Diffusive flux of component k in fluid phase alpha.
 */
class ComponentDiffusiveFlux : public Kernel
{
public:
  ComponentDiffusiveFlux(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  /// Member reference to the diffusivity of component k in phase alpha material properties
  const MaterialProperty<std::vector<Real> > & _diffusivity;

private:
  VariableValue & _fluid_density;
  VariableGradient & _grad_component_mass_fraction;

  /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;

  /// Index of the fluid phase that this kernel acts on
  unsigned int _phase_index;
  /// Index of the component that this kernel acts on
  unsigned int _component_index;
  /// Variable number of the mass fraction variable
  unsigned int _xvar;
  /// Index of the diffusivity for this component in this phase
  unsigned int _diffusivity_index;
  /// Primary variable type (used in Jacobian calculations)
  std::string _primary_variable_type;
  /**
  * The sign of the derivative of the mass fraction variable that this kernel
  * acts on wrt the primary mass fraction variable
   */
  unsigned int _sgnx;


};

#endif //COMPONENTDIFFUSIVEFLUX_H
