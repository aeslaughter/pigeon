/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidStateWaterCO2.h"

template<>
InputParameters validParams<FluidStateWaterCO2>()
{
  InputParameters params = validParams<FluidStateTwoPhase>();
  params.addClassDescription("Thermophysical fluid state of water (H2O) and CO2.");
  /// Suppress fluid property UserObjects from FluidStateTwoPhase and use water and CO2 specialisations
  //params.suppressParameter<UserObjectName>("liquid_property_uo");
  //params.suppressParameter<UserObjectName>("gas_property_uo");
//  params.addRequiredParam<UserObjectName>("water_property_uo", "Name of the User Object defining the water properties");
//  params.addRequiredParam<UserObjectName>("co2_property_uo", "Name of the User Object defining the CO2 properties");
  return params;
}

FluidStateWaterCO2::FluidStateWaterCO2(const InputParameters & parameters) :
  FluidStateTwoPhase(parameters),

  _water_property(getUserObject<FluidPropertiesWater>("liquid_property_uo")),
  _co2_property(getUserObject<FluidPropertiesCO2>("gas_property_uo"))
{
  _Mh2o = _water_property.molarMass();
  _Mco2 = _co2_property.molarMass();

  _num_components = 2;
  _num_phases = 2;
}

void
FluidStateWaterCO2::thermophysicalProperties(std::vector<Real> primary_vars, FluidStateProperties & fsp)
{
  /// Primary variables at the node
  Real node_pressure = primary_vars[0];
  Real node_temperature = primary_vars[1];
  Real node_saturation = primary_vars[2];
  Real node_xmass = primary_vars[3];

  /// Assign the fluid properties
  /// Saturation
  fsp.saturation = saturation(node_saturation);

  /// Pressure (takes liquid saturation for capillary pressure calculation)
  fsp.pressure = pressure(node_pressure, fsp.saturation[0]);

  // The vapour pressure of H2O
  Real pv = _water_property.pSat(node_temperature);

  // The partial pressure of the CO2 in the gas phase
  Real co2_partial_pressure = fsp.pressure[1] - pv;

  // The mass fraction of CO2 in the liquid phase
  Real xco2l = dissolved(co2_partial_pressure, node_temperature);

  // The density and viscosity of the liquid phase water. If the saturation is zero,
  // set the phase density to zero (keep viscosity non-zero as it appears in the denominator
  // of the mobility.)
  Real liquid_density, water_viscosity;

  if (fsp.saturation[0] == 0.)
  {
    liquid_density = 0.;
    water_viscosity = 1.; // Not used but set to a non-zero value
  }
  else
  {
    Real water_density = _water_property.density(fsp.pressure[0], node_temperature);
    water_viscosity = _water_property.viscosity(fsp.pressure[0], node_temperature, water_density);

    liquid_density = 1.0 / (xco2l / _co2_property.partialDensity(node_temperature) +
      (1.0 - xco2l) / water_density);
  }

  // The density of the gas phase, and the mass fraction of CO2 in the gase phase. If the
  // saturation is zero, set the phase density to zero (keep viscosity non-zero as it appears
  // in the denominator of the mobility.)
  Real gas_density, xco2g, co2_viscosity, vapour_viscosity;

  if (fsp.saturation[1] == 0.)
  {
    gas_density = 0.;
    xco2g = 0.;
    co2_viscosity = 1.;  // Not used
    vapour_viscosity = 1.;  // Not used
  }
  else
  {
    Real co2_density = _co2_property.density(co2_partial_pressure, node_temperature);
    Real vapour_density = _water_property.density(pv, node_temperature);
    gas_density = co2_density + vapour_density;
    xco2g = co2_density / gas_density;
    co2_viscosity = _co2_property.viscosity(co2_partial_pressure, node_temperature, co2_density);
    vapour_viscosity = _water_property.viscosity(pv, node_temperature, vapour_density);
  }

  // Store the density of each phase
  std::vector<Real> densities;

  densities.push_back(liquid_density);
  densities.push_back(gas_density);
  fsp.density = densities;

  // Store the viscosity of each phase
  std::vector<Real> viscosities;
  viscosities.push_back(water_viscosity);
  viscosities.push_back(co2_viscosity);
  fsp.viscosity = viscosities;

  // Store the relative permeability of each phase
  fsp.relperm = relativePermeability(fsp.saturation[0]);

  // Store the mass fraction of each component in each phase
  std::vector<std::vector<Real> > xmass;
  xmass.resize(numComponents());

  xmass[0].push_back(1.0 - xco2l); // H20 in liquid
  xmass[0].push_back(1.0 - xco2g); // H20 in gas
  xmass[1].push_back(xco2l); // CO2 in liqiud
  xmass[1].push_back(xco2g); // CO2 in gas

  fsp.mass_fraction = xmass;

  /// Mobility of each phase
  std::vector<Real> mobilities(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    mobilities[i] = fsp.relperm[i] * fsp.density[i]  / fsp.viscosity[i];

  fsp.mobility = mobilities;

  /// For derivatives wrt saturation, the sign of the derivative depends on whether the primary
  /// saturation variable is liquid or not. This can be accounted for by multiplying all derivatives
  /// wrt S by dS/dSl
  Real sgn = dSaturation_dSl(_svar);

  /// Derivative of relative permeability wrt liquid_saturation
  std::vector<Real> drelperm(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    drelperm[i] = sgn * dRelativePermeability(fsp.saturation[0])[i];

  fsp.drelperm = drelperm;

  /// Derivative of density wrt pressure
  std::vector<Real> ddensities_dp(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    ddensities_dp[i] = dDensity_dP(fsp.pressure[i], node_temperature, i);

  fsp.ddensity_dp = ddensities_dp;

  /// Derivative of density wrt saturation
  std::vector<Real> ddensities_ds(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    ddensities_ds[i] = sgn * dCapillaryPressure(fsp.saturation[0])[i] * fsp.ddensity_dp[i];

  fsp.ddensity_ds = ddensities_ds;

  /// Derivative of density wrt mass fraction
  std::vector<std::vector<Real> > ddensities_dx;
  ddensities_dx.resize(numComponents());

  ddensities_dx[0].push_back(0.);
  ddensities_dx[0].push_back(0.);
  ddensities_dx[1].push_back(0.);
  ddensities_dx[1].push_back(0.);

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

Real
FluidStateWaterCO2::dDensity_dP(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real dfluid_density;

  if (phase_index == 0) /// liquid phase
    dfluid_density = _water_property.dDensity_dP(pressure, temperature);
  else if (phase_index == 1) /// gas phase
    dfluid_density = _co2_property.dDensity_dP(pressure, temperature);
  else
    mooseError("phase_index " << phase_index << " is out of range in FluidStateWaterCO2::dDensity_dP");

  return dfluid_density;
}

Real
FluidStateWaterCO2::dDensity_dX(Real pressure, Real temperature, unsigned int phase_index) const
{
  ///TODO: Not implemented
  Real dfluid_density = 0.0;

  return dfluid_density;
}

Real
FluidStateWaterCO2::henry(Real temperature) const
{
  // Check temperature to make sure that it isn't out of the region of validity
  if (temperature < 1.04 || temperature > 369.51)
    mooseError("The temperature is out of range in FluidStateWaterCO2::henry");

  Real t_critical = 647.096;
  Real t_c2k = 273.15;
  std::vector<Real> a = _co2_property.henryConstants();
  Real tr = (temperature + t_c2k) / t_critical;

  Real kh = a[0] / tr + (a[1] / tr) * std::pow(1.0 - tr, 0.355) + a[2] *
            std::pow(tr, -0.41) * std::exp(1.0 - tr);

  return _water_property.pSat(temperature) * std::exp(kh);
}

Real
FluidStateWaterCO2::dissolved(Real pressure, Real temperature) const
{
  // Calculate the dissolved mole fraction of CO2 in water
  Real xco2lm = pressure / FluidStateWaterCO2::henry(temperature);
  // The mass fraction is then
  Real xco2l = xco2lm * _Mco2 / (xco2lm * _Mco2 + (1.0 - xco2lm) * _Mh2o);

  return xco2l;
}

Real
FluidStateWaterCO2::density(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real fluid_density;
  // The vapour pressure of H2O
  Real pv = _water_property.pSat(temperature);
  // The partial pressure of the CO2 in the gas phase
  Real co2_partial_pressure = pressure - pv;

  if (phase_index == 0) // Liquid phase
  {
    fluid_density = _water_property.density(pressure, temperature);

    // The mass fraction of CO2 in the liquid phase
    Real xco2l = dissolved(co2_partial_pressure, temperature);

    // The liquid density increase due to dissolved co2
    fluid_density = 1.0 / (xco2l / _co2_property.partialDensity(temperature) +
      (1.0 - xco2l) / fluid_density);
  }

  else if (phase_index == 1) // Gas phase
  {
    // The density and viscosity of the gas phase water
    Real vapour_density = _water_property.density(pv, temperature);

    // The density and viscosity of the gas phase CO2
    Real co2_density = _co2_property.density(co2_partial_pressure, temperature);

    // The density of the gas is found from the sum of densities for each component
    fluid_density = co2_density + vapour_density;
  }

  else
    mooseError("phase_index is out of range in FluidStateWaterCO2::density");

  return fluid_density;
}

Real
FluidStateWaterCO2::viscosity(Real pressure, Real temperature, Real density, unsigned int phase_index) const
{
  Real fluid_viscosity;

  if (phase_index == 0)
    fluid_viscosity = _water_property.viscosity(pressure, temperature, density);

  else if (phase_index == 1)
    fluid_viscosity = _co2_property.viscosity(pressure, temperature, density);

  else
    mooseError("phase_index is out of range in FluidStateWaterCO2::viscosity");

  // TODO: effect of co2 in water on viscosity

  return fluid_viscosity;
}

Real
FluidStateWaterCO2::dViscosity_dDensity(Real pressure, Real temperature, Real density, unsigned int phase_index) const
{
  Real dviscosity_ddensity;

  if (phase_index == 0) /// liquid phase
    dviscosity_ddensity = _water_property.dViscosity_dDensity(pressure, temperature, density);
  else if (phase_index == 1) /// gas phase
    dviscosity_ddensity = _co2_property.dViscosity_dDensity(pressure, temperature, density);
  else
    mooseError("phase_index " << phase_index << " is out of range in FluidStateWaterCO2::dViscosity_dDensity");

  return dviscosity_ddensity;
}
