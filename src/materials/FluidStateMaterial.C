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
  params.addRequiredCoupledVar("fluid_density_variables", "The density of each fluid phase");

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
    _pvar(coupled("primary_pressure_variable")),
    _svar(coupled("primary_saturation_variable")),
    _fluid_state(getUserObject<FluidState>("fluid_state_uo"))

{
  // The number of phases in the given fluid state model
  _num_phases = _fluid_state.numPhases();

  // Check that the required number of auxillary variables have been provided
  if (coupledComponents("fluid_density_variables") != _num_phases)
    mooseError("The number of phase densities provided in FluidStateMaterial is not equal to the number of phases in the FluidState UserObject");

  // Determine the phase of the primary variable that this Kernel acts on
  _p_phase = _fluid_state.variablePhase(_pvar);
  _s_phase = _fluid_state.variablePhase(_svar);

}

void
FluidStateMaterial::computeQpProperties()
{
  Real temperature;

  // Check if the simulation is isothermal
  if (_fluid_state.isIsothermal())
    temperature = _fluid_state.isothermalTemperature();
  else
    temperature = _temperature[_qp];


  std::vector<Real> pressure, saturation;

  // Compute pressure, saturation and density at the qp's
  saturation = _fluid_state.saturation(_primary_saturation[_qp], _s_phase);
  Real liquid_saturation = saturation[0];
  pressure = _fluid_state.pressure(_primary_pressure[_qp], liquid_saturation, _p_phase);

  _phase_mass[_qp].resize(_num_phases);
  _density.resize(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    _density[n] = &coupledValue("fluid_density_variables", n);
    _phase_mass[_qp][n] = (*_density[n])[_qp] * saturation[n];
  }

  _phase_flux_no_mobility[_qp].resize(_num_phases);
  _dphase_flux_no_mobility_dp[_qp].resize(_num_phases);
  _dphase_flux_no_mobility_ds[_qp].resize(_num_phases);

  RealVectorValue grad_pressure;

  // Depending on the primary saturation, the sign of the gradient in saturation is + or -
  Real sgn = _fluid_state.dSaturation_dSl(_s_phase);

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    // The pressure gradient of phase n
    grad_pressure = _grad_primary_pressure[_qp] + _fluid_state.dCapillaryPressure(liquid_saturation)[n] *
      sgn * _grad_primary_saturation[_qp];

    // Flux (without mobility) of phase n
    _phase_flux_no_mobility[_qp][n] = (grad_pressure - (*_density[n])[_qp] * _gravity[_qp]);

    // Derivative of flux (without mobility) of phase n wrt pressure of phase n
    _dphase_flux_no_mobility_dp[_qp][n] = - _fluid_state.dDensity_dP(pressure[n], temperature, n) * _gravity[_qp];

    // Derivative of flux (without mobility) of phase n wrt saturation of phase n (through capillary pressure)
    _dphase_flux_no_mobility_ds[_qp][n] = - _fluid_state.dCapillaryPressure(liquid_saturation)[n] *
      sgn * _grad_primary_saturation[_qp];
  }
}
