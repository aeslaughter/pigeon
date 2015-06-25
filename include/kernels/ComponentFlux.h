/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef COMPONENTFLUX_H
#define COMPONENTFLUX_H

#include "Kernel.h"

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
  virtual void computeOffDiagJacobian();
  /**
   * Function to do upwinding for both the Residual and Jacobian
   */
  void upwind(bool compute_res, bool compute_jac, unsigned int jvar);

  // Member reference to the porosity and permeability material properties
  const MaterialProperty<RealTensorValue> & _permeability;
  const MaterialProperty<RealVectorValue> & _gravity;

  // Member reference to phase flux (without mobility) material properties
  const MaterialProperty<std::vector<RealGradient> > & _phase_flux_no_mobility;

private:
  VariableValue & _fluid_density;
  VariableValue & _fluid_viscosity;
  VariableValue & _component_mass_fraction;
  VariableValue & _fluid_relperm;
  const MooseEnum & _primary_variable_type;


  unsigned int _fluid_pressure_var;
  unsigned int _num_phases;
  unsigned int _phase_index;
};

#endif //COMPONENTFLUX_H
