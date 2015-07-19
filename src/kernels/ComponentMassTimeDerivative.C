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
  params.addRequiredCoupledVar("fluid_density_variable", "The fluid density variable for this phase.");
  params.addRequiredCoupledVar("fluid_saturation_variable", "The fluid saturation variable for this phase.");
  params.addRequiredCoupledVar("fluid_pressure_variable", "The fluid pressure variable for this phase.");
  params.addCoupledVar("temperature_variable", 50,  "The temperature variable.");
  params.addCoupledVar("component_mass_fraction_variable", 1.0, "The component mass fraction variable for this phase.");
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state");
  return params;
}

ComponentMassTimeDerivative::ComponentMassTimeDerivative(const std::string & name,
                       InputParameters parameters) :
  TimeKernel(name, parameters),
  _porosity(getMaterialProperty<Real>("porosity")),
  _fluid_density(coupledNodalValue("fluid_density_variable")),
  _fluid_density_old(coupledNodalValueOld("fluid_density_variable")),
  _fluid_saturation(coupledNodalValue("fluid_saturation_variable")),
  _fluid_saturation_old(coupledNodalValueOld("fluid_saturation_variable")),
  _component_mass_fraction(coupledNodalValue("component_mass_fraction_variable")),
  _component_mass_fraction_old(coupledNodalValueOld("component_mass_fraction_variable")),
  _fluid_pressure(coupledNodalValue("fluid_pressure_variable")),
  _fluid_pressure_old(coupledNodalValueOld("fluid_pressure_variable")),
  _temperature(coupledNodalValue("temperature_variable")),
//  _temperature(getVar("temperature_variable", 0)),
  _fluid_state(getUserObject<FluidState>("fluid_state_uo"))

{
  if (!_fluid_state.isFluidStateVariable(_var.number()))
    mooseError("Variable " << _var.name() << " in the " << _name << " kernel is not a FluidState variable");

  // Determine the primary variable type
  _primary_variable_type = _fluid_state.variableTypes(_var.number());

  // Determine the phase of the primary variable that this Kernel acts on
  _phase_index = _fluid_state.variablePhase(_var.number());
  
}

// Note that this kernel lumps the mass terms to the nodes, so that there is no mass at the qp's.
Real
ComponentMassTimeDerivative::computeQpResidual()
{
  Real mass = 0.;
  Real mass_old = 0.;

   mass += _component_mass_fraction[_i] * _fluid_density[_i] * _fluid_saturation[_i];
   mass_old += _component_mass_fraction_old[_i] * _fluid_density_old[_i] * _fluid_saturation_old[_i];

  //TODO: allow for porosity change
  return _test[_i][_qp] * _porosity[_qp] * (mass - mass_old)/_dt;
}

Real
ComponentMassTimeDerivative::computeQpJacobian() // TODO: Jacobians need further work!
{
  if (_i != _j)
    return 0.0;

  Real qpjacobian = 0.;

    if (_primary_variable_type == "saturation")
      qpjacobian =  _component_mass_fraction[_i] * _fluid_density[_i];

    if (_primary_variable_type == "pressure")
    {
      Real dDensity_dP = _fluid_state.dDensity_dP(_fluid_pressure[_i], _temperature[_i])[_phase_index];
      qpjacobian = _component_mass_fraction[_i] * _fluid_saturation[_i] * dDensity_dP;
    }

    if (_primary_variable_type == "mass_fraction")
    {
      //TODO: properly implement this
      qpjacobian = _fluid_density[_i] * _fluid_saturation[_i];
    }

  return _test[_i][_qp] * _porosity[_qp] * qpjacobian / _dt;
}

Real
ComponentMassTimeDerivative::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (_i != _j)
    return 0.0;

  if (!_fluid_state.isFluidStateVariable(jvar))
    return 0.0;

  // Determine the variable type to take the derivative with respect to
  std::string jvar_type = _fluid_state.variableTypes(jvar);

  Real qpoffdiagjacobian = 0.;

    if (_primary_variable_type == "saturation")
      if (jvar_type == "pressure")
      {
        Real dDensity_dP = _fluid_state.dDensity_dP(_fluid_pressure[_i], _temperature[_i])[_phase_index];
        qpoffdiagjacobian = _component_mass_fraction[_i] * _fluid_saturation[_i] * dDensity_dP;
      }

    if (_primary_variable_type == "pressure")
      if (jvar_type == "saturation")
        qpoffdiagjacobian =  _component_mass_fraction[_i] * _fluid_density[_i];

    if (_primary_variable_type == "mass_fraction")
    {
      //TODO: properly implement this
      qpoffdiagjacobian = _fluid_density[_i] * _fluid_saturation[_i];
    }

  return _test[_i][_qp] * _porosity[_i] * qpoffdiagjacobian / _dt;
}
