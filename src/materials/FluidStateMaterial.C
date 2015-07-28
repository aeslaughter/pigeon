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
  params.addRequiredCoupledVar("primary_mass_fraction_variable", "The primary mass fraction variable");
  params.addRequiredCoupledVar("temperature_variable", "The temperature variable");
  params.addRequiredCoupledVar("fluid_density_variables", "The density of each fluid phase");
  params.addParam<std::vector<Real> >("diffusivity", "Vector of diffusivity for each component in each phase. Order is i) component 1 in phase 1; ii) component 1 in phase 2 ...; component 2 in phase 1; ... component n in phase m (m^2/s");

  return params;
}

FluidStateMaterial::FluidStateMaterial(const InputParameters & parameters) :
    Material(parameters),

    // Get gravity from PorousMaterial class
    _gravity(getMaterialProperty<RealVectorValue>("gravity")),
    _material_diffusivity(getParam<std::vector<Real> >("diffusivity")),

    // Declare vector of phase fluxes (without mobility)
    _phase_flux_no_mobility(declareProperty<std::vector<RealGradient> >("phase_flux_no_mobility")),
    _phase_mass(declareProperty<std::vector<Real> >("phase_mass")),
    _dpressure_flux_dp(declareProperty<std::vector<Real> >("dpressure_flux_dp")),
    _dpressure_flux_ds(declareProperty<std::vector<Real> >("dpressure_flux_ds")),
    _dgravity_flux_dp(declareProperty<std::vector<RealVectorValue> >("dgravity_flux_dp")),
    _dgravity_flux_ds(declareProperty<std::vector<RealVectorValue> >("dgravity_flux_ds")),
    _dgravity_flux_dx(declareProperty<std::vector<std::vector<RealVectorValue> > >("dgravity_flux_dx")),
    _diffusivity(declareProperty<std::vector<Real> >("diffusivity")),

    _primary_saturation(coupledValue("primary_saturation_variable")),
    _grad_primary_saturation(coupledGradient("primary_saturation_variable")),
    _primary_pressure(coupledValue("primary_pressure_variable")),
    _grad_primary_pressure(coupledGradient("primary_pressure_variable")),
    _temperature(coupledValue("temperature_variable")),
    _primary_mass_fraction(coupledValue("primary_mass_fraction_variable")),
    _pvar(coupled("primary_pressure_variable")),
    _svar(coupled("primary_saturation_variable")),
    _xvar(coupled("primary_mass_fraction_variable")),
    _fluid_state(getUserObject<FluidState>("fluid_state_uo"))

{
  /// The number of phases in the given FluidState model
  _num_phases = _fluid_state.numPhases();

  /// The number of components in the given FluidState model
  _num_components = _fluid_state.numComponents();

  /// Check that the required number (_num_phases) of auxillary variables have been provided
  if (coupledComponents("fluid_density_variables") != _num_phases)
    mooseError("The number of phase densities provided in FluidStateMaterial is not equal to the number of phases in the FluidState UserObject");

  /// Check that the required number (_num_phases * _num_components) of diffusivities have been provided
  if (_material_diffusivity.size() != _num_phases * _num_components)
    mooseError("The number of diffivities given in FluidStateMaterial, " << _material_diffusivity.size() <<", is not equal to the required number " << _num_phases * _num_components);

  /// Determine the phase of the primary variable that this material acts on
  _p_phase = _fluid_state.variablePhase(_pvar);
  _s_phase = _fluid_state.variablePhase(_svar);

}

void
FluidStateMaterial::computeQpProperties()
{
  Real temperature;

  _diffusivity[_qp] = _material_diffusivity;

  /// Check if the simulation is isothermal
  if (_fluid_state.isIsothermal())
    temperature = _fluid_state.isothermalTemperature();
  else
    temperature = _temperature[_qp];


  std::vector<Real> pressure, saturation;

  /// Compute pressure, saturation and density at the qp's
  saturation = _fluid_state.saturation(_primary_saturation[_qp]);
  Real liquid_saturation = saturation[0];
  pressure = _fluid_state.pressure(_primary_pressure[_qp], liquid_saturation);

  _phase_mass[_qp].resize(_num_phases);
  _density.resize(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    _density[n] = &coupledValue("fluid_density_variables", n);
    _phase_mass[_qp][n] = (*_density[n])[_qp] * saturation[n];
  }

  /// Initialise the size of the material property vectors
  _phase_flux_no_mobility[_qp].resize(_num_phases);
  _dpressure_flux_dp[_qp].resize(_num_phases);
  _dpressure_flux_ds[_qp].resize(_num_phases);
  _dgravity_flux_dp[_qp].resize(_num_phases);
  _dgravity_flux_ds[_qp].resize(_num_phases);
  _dgravity_flux_dx[_qp].resize(_num_components);

  RealVectorValue grad_pressure;

  /// Depending on the primary saturation, the sign of the gradient in saturation is + or -
  Real sgns;

  /// Loop over all phases and calculate the phase flux and derivatives
  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    /// The sign of the gradient in saturation
    sgns = _fluid_state.dSaturation_dSl(n);

    /// The pressure gradient of phase n
    grad_pressure = _grad_primary_pressure[_qp] + sgns * _fluid_state.dCapillaryPressure(liquid_saturation)[n] *
      _grad_primary_saturation[_qp];

    /// Advective flux (without mobility) of phase n
    _phase_flux_no_mobility[_qp][n] = (grad_pressure - (*_density[n])[_qp] * _gravity[_qp]);

    /// Derivative of gravity flux (density * gravity) of phase n wrt pressure
    _dgravity_flux_dp[_qp][n] = - _fluid_state.dDensity_dP(pressure[n], temperature, n) * _gravity[_qp];

    /// Derivative of gravity flux (density * gravity) of phase n wrt saturation
    _dgravity_flux_ds[_qp][n] = sgns * _fluid_state.dCapillaryPressure(liquid_saturation)[n] * _dgravity_flux_dp[_qp][n];

    /// Derivative of pressure flux (GradP) of phase n wrt pressure
    _dpressure_flux_dp[_qp][n] = 1.0;

    /// Derivative of pressure flux (GradP) of phase n wrt pressure
    _dpressure_flux_ds[_qp][n] = sgns * _fluid_state.dCapillaryPressure(liquid_saturation)[n];
  }

  /// The sign of the derivative of flux wrt mass fraction
  Real sgnx;

  /// Loop over all components and phases and calculate derivative of the flux wrt mass fraction
  for (unsigned int i = 0; i < _num_components; ++i)
  {
    // Check if the compnent index is the primary component index and set the sign of the derivative appropriately
    sgnx = (i == _fluid_state.primaryComponentIndex() ? 1.0 : -1.0);

    for (unsigned int n = 0; n < _num_phases; ++n)
      _dgravity_flux_dx[_qp][i].push_back(sgnx * _fluid_state.dDensity_dX(pressure[n], temperature, n));
  }



}
