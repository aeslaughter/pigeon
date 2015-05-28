/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef COMPONENTFLUX_H
#define COMPONENTFLUX_H

#include "Kernel.h"
#include "FluidState.h"

class ComponentFlux;

template<>
InputParameters validParams<ComponentFlux>();

class ComponentFlux : public Kernel
{
public:
  ComponentFlux(const std::string & name, InputParameters parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);
  virtual void computeResidual();
  virtual void computeJacobian();
  /**
   * Function to do upwinding for both the Residual and Jacobian
   */
  void upwind(bool compute_res, bool compute_jac, unsigned int jvar);

  // Member reference to the porosity and permeability material properties
  MaterialProperty<RealTensorValue> & _permeability;
  MaterialProperty<RealVectorValue> & _gravity;

  // Member reference to phase flux (without mobility) material properties
  MaterialProperty<std::vector<RealVectorValue> > & _phase_flux_no_mobility;

private:
  std::vector<VariableValue *> _fluid_density;
  std::vector<VariableValue *> _fluid_viscosity;
  std::vector<VariableValue *> _component_mass_fraction;
  std::vector<VariableValue *> _fluid_relperm;
  const MooseEnum & _primary_variable_type;

  /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;
  unsigned int _fluid_pressure_var;
  unsigned int _num_phases;
  unsigned int _phase_index;
};

#endif //COMPONENTFLUX_H
