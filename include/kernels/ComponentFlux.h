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
  virtual void computeOffDiagJacobian(unsigned int jvar);
  /**
   * Function to do upwinding for both the Residual and Jacobian
   */
  void upwind(bool compute_res, bool compute_jac, unsigned int jvar);

  // Member reference to the porosity and permeability material properties
  const MaterialProperty<RealTensorValue> & _permeability;
  const MaterialProperty<RealVectorValue> & _gravity;

  // Member reference to phase flux (without mobility) material properties
  const MaterialProperty<std::vector<RealGradient> > & _phase_flux_no_mobility;
  const MaterialProperty<std::vector<RealVectorValue> > & _dgravity_flux_dp;
  const MaterialProperty<std::vector<RealVectorValue> > & _dgravity_flux_ds;
  const MaterialProperty<std::vector<Real> > & _dpressure_flux_dp;
  const MaterialProperty<std::vector<Real> > & _dpressure_flux_ds;


private:
//  VariableValue & _fluid_pressure;
//  VariableValue & _fluid_saturation;
//  VariableValue & _temperature;
//  VariableValue & _fluid_density;
//  VariableValue & _fluid_viscosity;
  VariableValue & _component_mass_fraction;
//  VariableValue & _fluid_relperm;

  /**
   * This is the member reference that will hold the User Object
   * value for the fluid state User Object.
   */
  const FluidState & _fluid_state;

  unsigned int _phase_index;
  std::string _primary_variable_type;

};

#endif //COMPONENTFLUX_H