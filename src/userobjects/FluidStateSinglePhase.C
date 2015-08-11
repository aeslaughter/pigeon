/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidStateSinglePhase.h"

template<>
InputParameters validParams<FluidStateSinglePhase>()
{
  InputParameters params = validParams<FluidState>();
  params.addClassDescription("Thermophysical fluid state of single phase fluid");
  params.addRequiredParam<UserObjectName>("fluid_property_uo", "Name of the User Object defining the fluid properties");
  params.addParam<Real>("component_density_increase", 0., "The increase in density with dissolved component 1");
  return params;
}

FluidStateSinglePhase::FluidStateSinglePhase(const InputParameters & parameters) :
  FluidState(parameters),

  _fluid_property(getUserObject<FluidProperties>("fluid_property_uo")),
  _density_increase(getParam<Real>("component_density_increase"))

{
  /// Only one phase in this FluidState
  _num_phases = 1;
}

unsigned int
FluidStateSinglePhase::numPhases() const
{
  return _num_phases;
}

unsigned int
FluidStateSinglePhase::variablePhase(unsigned int moose_var) const
{
 /// Only one phase in this FluidState
  return 0;
}

void
FluidStateSinglePhase::thermophysicalProperties(std::vector<Real> primary_vars, FluidStateProperties & fsp)
{
  /// Primary variables at the node
  Real node_pressure = primary_vars[0];
  Real node_temperature = primary_vars[1];
  Real node_saturation = primary_vars[2];
  Real node_xmass = primary_vars[3];

  /// Assign the fluid properties
  fsp.saturation = saturation(node_saturation);

  /// Pressure (takes liquid saturation for capillary pressure calculation)
  fsp.pressure = pressure(node_pressure, fsp.saturation[0]);

  /// Density FIXME: Need to generalise density of saturated mixture
  std::vector<Real> densities(_num_phases);
  Real density0 = _fluid_property.density(fsp.pressure[0], node_temperature);
  /// Now add increase due to dissolved component 1
  densities[0] = density0 * (density0 + _density_increase) / (density0 + _density_increase - node_xmass * _density_increase);

  fsp.density = densities;

  /// Viscosity
  std::vector<Real> viscosities(_num_phases);

  /// Water viscosity uses water density in calculation.
  viscosities[0] = _fluid_property.viscosity(node_pressure, node_temperature, fsp.density[0]);

  fsp.viscosity = viscosities;

  /// Relative permeability (Equal to 1)
  fsp.relperm = relativePermeability(fsp.saturation[0]);

  /// Mass fraction of each component
  std::vector<std::vector<Real> > xmass;
  xmass.resize(_num_components);

  xmass[_component_index].push_back(node_xmass); // primary mass fraction
  for (unsigned int i = 0; i < _num_components; ++i)
    if (i != _component_index)
      xmass[i].push_back(1.0 - node_xmass); // Other mass fraction

  fsp.mass_fraction = xmass;

  /// Mobility
  std::vector<Real> mobilities(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
    mobilities[n] = fsp.relperm[n] * fsp.density[n] / fsp.viscosity[n];

  fsp.mobility = mobilities;

  /// For derivatives wrt saturation, the sign of the derivative depends on whether the primary
  /// saturation variable is liquid or not. This can be accounted for by multiplying all derivatives
  /// wrt S by dS/dSl
  Real sgn = 1.0;

  /// Derivative of relative permeability wrt liquid_saturation
  std::vector<Real> drelperm(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
    drelperm[n] = sgn * dRelativePermeability(fsp.saturation[0])[n];

  fsp.drelperm = drelperm;

  /// Derivative of density wrt pressure
  std::vector<Real> ddensities_dp(_num_phases);

  ddensities_dp[0] = _fluid_property.dDensity_dP(fsp.pressure[0], node_temperature);

  fsp.ddensity_dp = ddensities_dp;

  /// Derivative of density wrt saturation
  std::vector<Real> ddensities_ds(_num_phases);

  for (unsigned int n = 0; n < _num_phases; ++n)
    ddensities_ds[n] = sgn * dCapillaryPressure(fsp.saturation[0])[n] * fsp.ddensity_dp[n];

  fsp.ddensity_ds = ddensities_ds;

  /// Derivative of density wrt mass fraction
  std::vector<std::vector<Real> > ddensities_dx;
  Real sgnx;
  ddensities_dx.resize(numComponents());
  //FIXME: need to fix this up properly
  Real ddx = _density_increase * fsp.density[0] / (density0 + _density_increase - node_xmass * _density_increase);
  for (unsigned int i = 0; i < _num_components; ++i)
  {
    sgnx = dMassFraction_dX(i);
    ddensities_dx[i].push_back(sgnx * ddx);
  }

  fsp.ddensity_dx = ddensities_dx;

  /// Derivative of mobility wrt pressure
  std::vector<Real> dmobilities_dp(_num_phases);
  Real dmdp;

  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    dmdp = (fsp.relperm[n] * fsp.ddensity_dp[n] / fsp.viscosity[n]) * (1.0 - (fsp.density[n] / fsp.viscosity[n]) *
      dViscosity_dDensity(fsp.pressure[n], node_temperature, fsp.density[n], n));
    dmobilities_dp[n] = dmdp;
  }

  fsp.dmobility_dp = dmobilities_dp;

  /// Derivative of mobility wrt saturation
  /// Note: drelperm and ddensity_ds are already the correct sign, so don't multiply by sgn
  std::vector<Real> dmobilities_ds(_num_phases);
  Real dmds;
  for (unsigned int n = 0; n < _num_phases; ++n)
  {
    dmds = fsp.drelperm[n] * fsp.density[n] / fsp.viscosity[n] + (fsp.relperm[n] * fsp.ddensity_ds[n] / fsp.viscosity[n]) * (1.0 - (fsp.density[n] / fsp.viscosity[n]) *
      dViscosity_dDensity(fsp.pressure[n], node_temperature, fsp.density[n], n));
    dmobilities_ds[n] = dmds;
  }

  fsp.dmobility_ds = dmobilities_ds;

  /// Derivative of mobility wrt mass fraction
  /// Note: ddensity_dx is already the correct sign, so don't multiply by sgn
  std::vector<std::vector<Real> > dmobilities_dx(_num_components);
  Real dmdx;
  for (unsigned int i = 0; i < _num_components; ++i)
    for (unsigned int n = 0; n < _num_phases; ++n)
    {
      dmdx = (fsp.relperm[n] * fsp.ddensity_dx[i][n] / fsp.viscosity[n]) * (1.0 - (fsp.density[n] / fsp.viscosity[n]) *
        dViscosity_dDensity(fsp.pressure[n], node_temperature, fsp.density[n], n));
      dmobilities_dx[i].push_back(dmdx);
    }
  fsp.dmobility_dx = dmobilities_dx;
}

std::vector<Real>
FluidStateSinglePhase::relativePermeability(Real liquid_saturation) const
{
  std::vector<Real> relperm;

  relperm.push_back(1.0);

  return relperm;
}

std::vector<Real>
FluidStateSinglePhase::dRelativePermeability(Real liquid_saturation) const
{
  std::vector<Real> drelperm;

  drelperm.push_back(0.0);

  return drelperm;
}

std::vector<Real>
FluidStateSinglePhase::pressure(Real pressure, Real liquid_saturation) const
{
  std::vector<Real> pressures;

  pressures.push_back(pressure);

  return pressures;
}

std::vector<Real>
FluidStateSinglePhase::dCapillaryPressure(Real liquid_saturation) const
{
  std::vector<Real> dpc;

  dpc.push_back(0.0);

  return dpc;
}

std::vector<Real>
FluidStateSinglePhase::d2CapillaryPressure(Real liquid_saturation) const
{
  std::vector<Real> d2pc;

  d2pc.push_back(0.0);

  return d2pc;
}

std::vector<Real>
FluidStateSinglePhase::saturation(Real saturation) const
{
  std::vector<Real> saturations;

  saturations.push_back(saturation);

  return saturations;
}

Real
FluidStateSinglePhase::dSaturation_dSl(unsigned int phase_index) const
{
  return 1.0;
}

Real
FluidStateSinglePhase::dSaturation_dS(unsigned int var) const
{
  return 1.0;
}

Real
FluidStateSinglePhase::dMassFraction_dX(unsigned int component_index) const
{
  return (component_index == _component_index ? 1.0 : -1.0);
}

Real
FluidStateSinglePhase::dDensity_dX(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real dfluid_density = 0.0;

  return dfluid_density;
}

Real
FluidStateSinglePhase::dViscosity_dDensity(Real pressure, Real temperature, Real density, unsigned int phase_index) const
{
  Real dviscosity_ddensity = _fluid_property.dViscosity_dDensity(pressure, temperature, density);

  return dviscosity_ddensity;
}
