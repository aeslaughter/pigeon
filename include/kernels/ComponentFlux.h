/****************************************************************/
/* Flux kernel for multiphase flow in porous media for each     */
/*  component.                                                  */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
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

private:
  std::vector<VariableValue *> _fluid_density;
  std::vector<VariableValue *> _fluid_viscosity;
  std::vector<VariableGradient *> _grad_fluid_pressure;
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
