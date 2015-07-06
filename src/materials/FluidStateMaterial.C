/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidStateMaterial.h"

template<>
InputParameters validParams<FluidStateMaterial>()
{
  InputParameters params = validParams<Material>();

  params.addRequiredParam<UserObjectName>("fluid_state_uo", "Name of the User Object defining the fluid state model");
  params.addRequiredCoupledVar("primary_saturation_variable", "The primary saturation variable");
  params.addRequiredCoupledVar("primary_pressure_variable", "The primary pressure variable");
  params.addCoupledVar("temperature_variable", 50., "The temperature variable");
  params.addParam<unsigned int>("phase_index", 0, "The phase index of the primary pressure variable");

  return params;
}

FluidStateMaterial::FluidStateMaterial(const std::string & name,
                                   InputParameters parameters) :
    Material(name, parameters),

    // Get gravity from PorousMaterial class
    _gravity(getMaterialProperty<RealVectorValue>("gravity")),

    // Declare vector of phase fluxes (without mobility)
    _phase_flux_no_mobility(declareProperty<std::vector<RealGradient> >("phase_flux_no_mobility")),
    _phase_mass(declareProperty<std::vector<Real> >("phase_mass")),
    _dphase_flux_no_mobility_dp(declareProperty<std::vector<RealVectorValue> >("dphase_flux_no_mobility_dp")),
    _dphase_flux_no_mobility_ds(declareProperty<std::vector<RealVectorValue> >("dphase_flux_no_mobility_ds")),

    _primary_saturation(coupledValue("primary_saturation_variable")),
    _grad_primary_saturation(coupledGradient("primary_saturation_variable")),
    _primary_pressure(coupledValue("primary_pressure_variable")),
    _grad_primary_pressure(coupledGradient("primary_pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _phase_index(getParam<unsigned int>("phase_index")),
    _fluid_state(getUserObject<FluidState>("fluid_state_uo"))

{
  // The number of phases in the given fluid state model
  _num_phases = _fluid_state.numPhases();
}

void
FluidStateMaterial::computeQpProperties()
{
  Real temperature;

  // Check if the simulation is isothermal
  if (_fluid_state.isIsothermal())
    temperature = _fluid_state.temperature();
  else
    temperature = _temperature[_qp];


  std::vector<Real> pressure, saturation, density;

  // Compute pressure, saturation and density at the qp's
  pressure = _fluid_state.pressure(_primary_pressure[_qp], _primary_saturation[_qp]);
  saturation = _fluid_state.saturation(_primary_saturation[_qp]);

  _phase_mass[_qp].resize(_num_phases);
  density.resize(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    density[n] = _fluid_state.density(pressure[n], temperature, n);
    _phase_mass[_qp][n] = density[n] * saturation[n];
  }

  _phase_flux_no_mobility[_qp].resize(_num_phases);
  _dphase_flux_no_mobility_dp[_qp].resize(_num_phases);

  RealVectorValue grad_pressure;

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    // If the saturation of this phase is zero, the phase density is zero, and
    // hence the phase flux is zero
    Real eps = 1.e-10;

    if (saturation[_phase_index] < eps)
      _phase_flux_no_mobility[_qp][n] = 0.;
    else
    {
      grad_pressure = _grad_primary_pressure[_qp] - _fluid_state.dCapillaryPressure(_primary_saturation[_qp])[n] *
      _grad_primary_saturation[_qp];

      _phase_flux_no_mobility[_qp][n] = (grad_pressure + density[n] * _gravity[_qp]);

      _dphase_flux_no_mobility_dp[_qp][n] = - _fluid_state.dDensity_dP(_primary_pressure[_qp], temperature)[n] * _gravity[_qp];

    }
  }
}
