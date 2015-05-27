/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "ComponentMassTimeDerivative.h"

template<>
InputParameters validParams<ComponentMassTimeDerivative>()
{
  InputParameters params = validParams<TimeKernel>();
  params.addRequiredCoupledVar("fluid_density_variables", "The list of fluid density auxiallary variables for each phase.");
  params.addRequiredCoupledVar("fluid_saturation_variables", "The list of fluid saturation variables for each phase.");
  params.addRequiredCoupledVar("fluid_pressure_variables", "The list of fluid pressure variables for each phase.");
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  params.addCoupledVar("component_mass_fraction_variables", 1.0, "The list of component mass fraction variables for each phase.");
  MooseEnum primary_variable_type("pressure saturation mass_fraction");
  params.addRequiredParam<MooseEnum>("primary_variable_type", primary_variable_type, "The type of primary variable for this kernel (e.g. pressure, saturation or component mass fraction");
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state");
  return params;
}

ComponentMassTimeDerivative::ComponentMassTimeDerivative(const std::string & name,
                       InputParameters parameters) :
    TimeKernel(name, parameters),
    _porosity(getMaterialProperty<Real>("porosity")),
    _primary_variable_type(getParam<MooseEnum>("primary_variable_type")),
    _temperature(coupledValue("temperature_variable")),
    _fluid_state(getUserObject<FluidState>("fluid_state_uo"))

{
  // The number of phases in the given fluid state model
  _num_phases = _fluid_state.numPhases();

  // Check that the required number of auxillary variables have been provided
  if (coupledComponents("fluid_density_variables") != _num_phases)
    mooseError("The number of phase densities provided in the ComponentMassTimeDerivative kernel is not equal to the number of phases in the FluidState UserObject");
  if (coupledComponents("fluid_saturation_variables") != _num_phases)
    mooseError("The number of phase saturations provided in the ComponentMassTimeDerivative kernel is not equal to the number of phases in the FluidState UserObject");
  if (coupledComponents("component_mass_fraction_variables") != _num_phases)
    mooseError("The number of phase components provided in the ComponentMassTimeDerivative kernel is not equal to the number of phases in the FluidState UserObject");
  if (coupledComponents("fluid_pressure_variables") != _num_phases)
    mooseError("The number of pressure variables provided in the ComponentMassTimeDerivative kernel is not equal to the number of phases in the FluidState UserObject");

  // Filling the vectors with VariableValue pointers to nodal values
  _fluid_density.resize(_num_phases);
  _fluid_density_old.resize(_num_phases);
  _fluid_saturation.resize(_num_phases);
  _fluid_saturation_old.resize(_num_phases);
  _component_mass_fraction.resize(_num_phases);
  _component_mass_fraction_old.resize(_num_phases);
  _fluid_pressure.resize(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    _fluid_density[n] = &coupledNodalValue("fluid_density_variables", n);
    _fluid_density_old[n] = &coupledNodalValueOld("fluid_density_variables", n);
    _fluid_saturation[n] = &coupledNodalValue("fluid_saturation_variables", n);
    _fluid_saturation_old[n] = &coupledNodalValueOld("fluid_saturation_variables", n);
    _component_mass_fraction[n] = &coupledNodalValue("component_mass_fraction_variables", n);
    _component_mass_fraction_old[n] = &coupledNodalValueOld("component_mass_fraction_variables", n);
    _fluid_pressure[n] = &coupledValue("fluid_pressure_variables");
  }

}

// Note that this kernel lumps the mass terms to the nodes, so that there is no mass at the qp's.
Real ComponentMassTimeDerivative::computeQpResidual()
{
  Real mass = 0.;
  Real mass_old = 0.;

  // Loop over all phases and sum the mass contributions
  for (unsigned int n = 0; n < _num_phases; ++n)
  {
   mass += (*_component_mass_fraction[n])[_i] * (*_fluid_density[n])[_i] * (*_fluid_saturation[n])[_i];
   mass_old += (*_component_mass_fraction_old[n])[_i] * (*_fluid_density_old[n])[_i] * (*_fluid_saturation_old[n])[_i];
  }

  //TODO: allow for porosity change
  return _test[_i][_qp] * _porosity[_qp] * (mass - mass_old)/_dt;
}

Real ComponentMassTimeDerivative::computeQpJacobian() // TODO: Jacobians need further work!
{
  // No off-diagonal mass entries in jacobian
  if (_i != _j)
    return 0.0;

  Real jacobian = 0.;
  // TODO: Currently only liquid saturation
  if (_primary_variable_type == "saturation") {
    jacobian = _test[_i][_qp] * _porosity[_qp] * (*_component_mass_fraction[0])[_i] * (*_fluid_density[0])[_i] / _dt;
  }

  // TODO: Currently only liquid pressure
  if (_primary_variable_type == "pressure") {

    Real dDensity_dP = _fluid_state.dDensity_dP((*_fluid_pressure[0])[_i], _temperature[_i])[0];
    jacobian = _test[_i][_qp] * _porosity[_i] * (*_component_mass_fraction[0])[_i] * (*_fluid_saturation[0])[_i] * dDensity_dP / _dt;
  }

  if (_primary_variable_type == "mass_fraction") {

    jacobian = _test[_i][_qp] * _porosity[_qp] * (*_fluid_density[0])[_i] * (*_fluid_saturation[0])[_i] / _dt;
  }

  return jacobian;
}

Real ComponentMassTimeDerivative::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.0;
}
