/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef COMPONENTADVECTIVEFLUX_H
#define COMPONENTADVECTIVEFLUX_H

#include "Kernel.h"
#include "FluidState.h"

class ComponentAdvectiveFlux;

template<>
InputParameters validParams<ComponentAdvectiveFlux>();

/**
 * Convective flux of component k in fluid phase alpha.
 * A fully-updwinded version is implemented, where the mobility
 * of the upstream nodes is used.
 */
class ComponentAdvectiveFlux : public Kernel
{
public:
  ComponentAdvectiveFlux(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);
  virtual void computeResidual();
  virtual void computeJacobian();
  virtual void computeOffDiagJacobian(unsigned int jvar);

  /**
   * Perform the upwinding for both the residual and Jacobians
   */
  void upwind(bool compute_res, bool compute_jac, unsigned int jvar);

  /// Member reference to the porosity and permeability material properties
  const MaterialProperty<RealTensorValue> & _permeability;
  const MaterialProperty<RealVectorValue> & _gravity;

  /// Member reference to phase flux (without mobility) material property
  const MaterialProperty<std::vector<RealGradient> > & _phase_flux_no_mobility;
  /// Derivative of the gravity component of the phase flux (without mobility) wrt phase pressure
  const MaterialProperty<std::vector<RealVectorValue> > & _dgravity_flux_dp;
  /// Derivative of the gravity component of the phase flux (without mobility) wrt phase saturation
  const MaterialProperty<std::vector<RealVectorValue> > & _dgravity_flux_ds;
  /// Derivative of the Grad P component of the phase flux (without mobility) wrt phase pressure
  const MaterialProperty<std::vector<Real> > & _dpressure_flux_dp;
  /// Derivative of the Grad P component of the phase flux (without mobility) wrt phase saturation
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

  /// Index of the fluid phase that this kernel acts on
  unsigned int _phase_index;
  /// Primary variable type (used in Jacobian calculations)
  std::string _primary_variable_type;

};

#endif //COMPONENTADVECTIVEFLUX_H
