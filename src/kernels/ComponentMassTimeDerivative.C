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
  params.addCoupledVar("temperature_variable", 50, "The temperature variable.");
  params.addCoupledVar("component_mass_fraction_variable", 1.0, "The component mass fraction variable for this phase.");
  MooseEnum primary_variable_type("pressure saturation mass_fraction");
  params.addRequiredParam<MooseEnum>("primary_variable_type", primary_variable_type, "The type of primary variable for this kernel (e.g. pressure, saturation or component mass fraction");
  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state");
  params.addParam<unsigned int>("phase_index", 0, "The index of the phase this kernel acts on");
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
    _primary_variable_type(getParam<MooseEnum>("primary_variable_type")),
    _temperature(coupledValue("temperature_variable")),
    _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
    _phase_index(getParam<unsigned int>("phase_index"))

{
}

// Note that this kernel lumps the mass terms to the nodes, so that there is no mass at the qp's.
Real ComponentMassTimeDerivative::computeQpResidual()
{
  Real mass = 0.;
  Real mass_old = 0.;

   mass += _component_mass_fraction[_i] * _fluid_density[_i] * _fluid_saturation[_i];
   mass_old += _component_mass_fraction_old[_i] * _fluid_density_old[_i] * _fluid_saturation_old[_i];

  // _console << "mass " << mass << std::endl;
  //TODO: allow for porosity change
  return _test[_i][_qp] * _porosity[_qp] * (mass - mass_old)/_dt;
}

Real ComponentMassTimeDerivative::computeQpJacobian() // TODO: Jacobians need further work!
{
  // No off-diagonal mass entries in jacobian
  if (_i != _j)
    return 0.0;

  Real qpjacobian;

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

  return _test[_i][_qp] * _porosity[_i] * qpjacobian / _dt;
}

Real ComponentMassTimeDerivative::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.0;
}
