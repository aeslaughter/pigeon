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
  params.addCoupledVar("primary_mass_fraction_variable", 1.0, "The primary mass fraction variable");
  params.addRequiredCoupledVar("temperature_variable", "The temperature variable");
  params.addParam<std::vector<Real> >("diffusivity", "Vector of diffusivity for each component in each phase. Order is i) component 1 in phase 1; ii) component 1 in phase 2 ...; component 2 in phase 1; ... component n in phase m (m^2/s");
  params.addParam<bool>("has_diffusion", false, "Is diffusion being modelled?");
  return params;
}

FluidStateMaterial::FluidStateMaterial(const InputParameters & parameters) :
    Material(parameters),

    // Get gravity from PorousMaterial class
    _gravity(getMaterialProperty<RealVectorValue>("gravity")),
    _material_diffusivity(getParam<std::vector<Real> >("diffusivity")),

    // Declare vector of phase fluxes (without mobility)
    _phase_flux_no_mobility(declareProperty<std::vector<RealGradient> >("phase_flux_no_mobility")),
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
    _has_diffusion(getParam<bool>("has_diffusion")),
    _fluid_state(getUserObject<FluidState>("fluid_state_uo")),
    _phi(_assembly.phi())

{
  /// The number of phases in the given FluidState model
  _num_phases = _fluid_state.numPhases();

  /// The number of components in the given FluidState model
  _num_components = _fluid_state.numComponents();

  /**
   * Check that the required number (_num_phases * _num_components) of diffusivities have been provided if the
   * has_diffusion flag is set to true. FIXME: Find way to not need flag to check size in all cases
   */
   if(_has_diffusion)
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

  /**
   * As density and its derivatives are calculated at the nodes in the FluidStateMaterial
   * UserObject, we will project their values onto the quadrature points using the shape
   * function _phi. This cuts down on code duplication in the FluidState UserObject, particularly
   * where density is a function of mass fraction of multiple components
   */

  Real qp_density = 0.;
  Real qp_ddensity_dp = 0.;
  Real node_density = 0.;
  Real node_ddensity_dp = 0.;

  /// Loop over all nodes on this element for each phase
  const Node * current_node;
  unsigned int nodeid;

  for (unsigned int j = 0; j < _num_phases; ++j)
  {
    for (unsigned int i = 0; i < _phi.size(); ++i)
    {
      current_node = _current_elem->get_node(i);
      nodeid = current_node->id();

      /// Density and required derivatives at the node
      node_density = _fluid_state.getNodalProperty("density", nodeid, j);
      node_ddensity_dp = _fluid_state.getNodalProperty("ddensity_dp", nodeid, j);

      //  Density and required derivatives at the qps
      qp_density += _phi[i][_qp] * node_density;
      qp_ddensity_dp += _phi[i][_qp] * node_ddensity_dp;
    }
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
  /// Derivative of capillary pressure wrt liquid saturation
  std::vector<Real> dcapillary_pressure = _fluid_state.dCapillaryPressure(liquid_saturation);
  /// Second derivative of capillary pressure wrt liquid saturation
  std::vector<Real> d2capillary_pressure = _fluid_state.d2CapillaryPressure(liquid_saturation);

  /// Loop over all phases and calculate the phase flux and derivatives
  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    /// The sign of the gradient in saturation
    sgns = _fluid_state.dSaturation_dS(n);

    /// The pressure gradient of phase n
    grad_pressure = _grad_primary_pressure[_qp] + sgns * dcapillary_pressure[n] *
      _grad_primary_saturation[_qp];

    /// Advective flux (without mobility) of phase n
    _phase_flux_no_mobility[_qp][n] = (grad_pressure - qp_density * _gravity[_qp]);
    /// Derivative of gravity flux (density * gravity) of phase n wrt pressure
    _dgravity_flux_dp[_qp][n] = - qp_ddensity_dp * _gravity[_qp];

    /// Derivative of gravity flux (density * gravity) of phase n wrt saturation
    /// Note: add d2(Pc)/dS2 term here, and note that the sign is correct (sgns * sgns = 1 always)
    _dgravity_flux_ds[_qp][n] = sgns * dcapillary_pressure[n] * _dgravity_flux_dp[_qp][n] +
      d2capillary_pressure[n] * _grad_primary_saturation[_qp];

    /// Derivative of pressure flux (GradP) of phase n wrt pressure
    _dpressure_flux_dp[_qp][n] = 1.0;

    /// Derivative of pressure flux (GradP) of phase n wrt saturation
    _dpressure_flux_ds[_qp][n] = sgns * dcapillary_pressure[n];
  }

  /// The sign of the derivative of flux wrt mass fraction
  Real sgnx;

  /// Loop over all components and phases and calculate derivative of the flux wrt mass fraction
  for (unsigned int i = 0; i < _num_components; ++i)
  {
    // FIXME
    // Check if the compnent index is the primary component index and set the sign of the derivative appropriately
    sgnx = (i == _fluid_state.primaryComponentIndex() ? 1.0 : -1.0);

    for (unsigned int n = 0; n < _num_phases; ++n)
      _dgravity_flux_dx[_qp][i].push_back(sgnx * _fluid_state.dDensity_dX(pressure[n], temperature, n) * _gravity[_qp]);
  }
}
