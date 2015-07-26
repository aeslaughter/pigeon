/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "ComponentDiffusiveFlux.h"

template<>
InputParameters validParams<ComponentDiffusiveFlux>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredCoupledVar("fluid_density_variable", "The fluid density auxiallary variable for this phase.");
  params.addRequiredCoupledVar("component_mass_fraction_variable", "The mass fraction of the kernels component in this phase.");
  params.addParam<unsigned int>("phase_index", 0, "The index corresponding to the fluid phase eg: 0 for liquid, 1 for gas");
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state");
  params.addClassDescription("Component Diffusive flux for component k in phase alpha");
  return params;
}

ComponentDiffusiveFlux::ComponentDiffusiveFlux(const InputParameters & parameters) :
  Kernel(parameters),
  _diffusivity(getMaterialProperty<RealVectorValue>("diffusivity")),
  _fluid_density(coupledValue("fluid_density_variable")),
  _grad_component_mass_fraction(coupledGradient("component_mass_fraction_variable")),
  _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
  _phase_index(getParam<unsigned int>("phase_index"))

{
  if (!_fluid_state.isFluidStateVariable(_var.number()))
    mooseError("Variable " << _var.name() << " in the " << _name << " kernel is not a FluidState variable");

  // Determine the primary variable type
  _primary_variable_type = _fluid_state.variableTypes(_var.number());

}

Real ComponentDiffusiveFlux::computeQpResidual()
{
  return _grad_test[_i][_qp] * _fluid_density[_qp] * _grad_component_mass_fraction[_qp];
}


Real ComponentDiffusiveFlux::computeQpJacobian()
{
  Real qpjacobian = 0.0;
  unsigned int nodeid = _current_elem->get_node(_i)->id();

  if (_primary_variable_type == "pressure")
  {
    Real dDensity_dP = _fluid_state.getNodalProperty("ddensity_dp", nodeid, _phase_index);
    qpjacobian = _grad_test[_i][_qp] * _phi[_j][_qp] * dDensity_dP * _grad_component_mass_fraction[_qp];
  }
  else if (_primary_variable_type == "saturation")
  {
    Real dDensity_dS = _fluid_state.getNodalProperty("ddensity_ds", nodeid, _phase_index);
    qpjacobian = _grad_test[_i][_qp] * _phi[_j][_qp] * dDensity_dS * _grad_component_mass_fraction[_qp];
  }
  else if (_primary_variable_type == "mass_fraction")
    qpjacobian = _grad_test[_i][_qp] * _fluid_density[_qp] * _grad_phi[_j][_qp];

  return qpjacobian;
}

Real ComponentDiffusiveFlux::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (!_fluid_state.isFluidStateVariable(jvar))
    return 0.0;

  Real qpoffdiagjacobian = 0.0;
  unsigned int nodeid = _current_elem->get_node(_i)->id();

  // Determine the variable type to take the derivative with respect to
  std::string jvar_type = _fluid_state.variableTypes(jvar);

  if (jvar_type == "pressure")
  {
    Real dDensity_dP = _fluid_state.getNodalProperty("ddensity_dp", nodeid, _phase_index);
    qpoffdiagjacobian = _grad_test[_i][_qp] * _phi[_j][_qp] * dDensity_dP * _grad_component_mass_fraction[_qp];
  }

  else if (jvar_type == "saturation")
  {
    Real dDensity_dS = _fluid_state.getNodalProperty("ddensity_ds", nodeid, _phase_index);
    qpoffdiagjacobian = _grad_test[_i][_qp] * _phi[_j][_qp] * dDensity_dS * _grad_component_mass_fraction[_qp];
  }

  else if (jvar_type == "mass_fraction")
    qpoffdiagjacobian = _grad_test[_i][_qp] * _fluid_density[_qp] * _grad_phi[_j][_qp];

  return qpoffdiagjacobian;
}
