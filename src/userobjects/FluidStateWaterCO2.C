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
  InputParameters params = validParams<FluidState>();
  params.addClassDescription("Thermophysical fluid state of water (H20) and CO2.");
  params.addRequiredParam<UserObjectName>("water_property_uo", "Name of the User Object defining the water properties");
  params.addRequiredParam<UserObjectName>("co2_property_uo", "Name of the User Object defining the CO2 properties");
  params.addRequiredParam<UserObjectName>("relative_permeability_uo", "Name of the User Object defining the relative permeabilities");
  params.addRequiredParam<UserObjectName>("capillary_pressure_uo", "Name of the User Object defining the capillary pressure");
  params.addParam<Real>("fluid_temperature", 20.0, "Isothermal fluid temperature");
  params.addParam<bool>("isothermal", false, "Is the simulations isothermal?");
  return params;
}

FluidStateWaterCO2::FluidStateWaterCO2(const std::string & name, InputParameters parameters) :
  FluidState(name, parameters),

  _water_property(getUserObject<FluidPropertiesWater>("water_property_uo")),
  _co2_property(getUserObject<FluidPropertiesCO2>("co2_property_uo")),
  _relative_permeability(getUserObject<RelativePermeability>("relative_permeability_uo")),
  _capillary_pressure(getUserObject<CapillaryPressure>("capillary_pressure_uo")),
  _fluid_temperature(getParam<Real>("fluid_temperature")),
  _is_isothermal(getParam<bool>("isothermal"))

{
  _Mh2o = _water_property.molarMass();
  _Mco2 = _co2_property.molarMass();
}

unsigned int
FluidStateWaterCO2::numPhases() const
{
  return 2;
}

unsigned int
FluidStateWaterCO2::numComponents() const
{
  return 2;
}

bool
FluidStateWaterCO2::isIsothermal() const
{
   return _is_isothermal;
}

Real
FluidStateWaterCO2::temperature() const
{
  return _fluid_temperature;
}

std::vector<std::string>
FluidStateWaterCO2::variable_names() const
{
  std::vector<std::string> varnames;
  varnames.push_back("gas_pressure");
  varnames.push_back("liquid_saturation");

  return varnames;
}

std::vector<std::string>
FluidStateWaterCO2::variable_types() const
{
  std::vector<std::string> varnames;
  varnames.push_back("pressure");
  varnames.push_back("saturation");

  return varnames;
}

std::vector<unsigned int>
FluidStateWaterCO2::variable_phase() const
{
  std::vector<unsigned int> varphases;
  varphases.push_back(1);  // gas phase
  varphases.push_back(0);  // liquid phase

  return varphases;
}

std::vector<std::vector<Real> >
FluidStateWaterCO2::thermophysicalProperties(Real pressure, Real temperature, Real saturation) const
{
  std::vector<std::vector<Real> > fluid_properties;
  fluid_properties.resize(numPhases());

  // Fix this: make it general regarding pressure and saturation.
  // At the moment, assumes gas pressure and liquid saturation

  // Gas and liquid phase saturations
  std::vector<Real> saturations = FluidStateWaterCO2::saturation(saturation);

  // The vapour pressure of H2O
  Real pv = _water_property.pSat(temperature);

  // The partial pressure of the CO2 in the gas phase
  Real co2_partial_pressure = pressure - pv;

  // The mass fraction of CO2 in the liquid phase
  Real xco2l = dissolved(co2_partial_pressure, temperature);

  // The density and viscosity of the liquid phase water. If the saturation is zero,
  // set the phase density to zero (keep viscosity non-zero as it appears in the denominator
  // of the mobility.)
  Real liquid_density, water_viscosity;

  if (saturations[0] == 0.)
  {
    liquid_density = 0.;
    water_viscosity = 1.; // Not used but set to a non-zero value
  }
  else
  {
    Real water_density = _water_property.density(pressure, temperature);
    water_viscosity = _water_property.viscosity(temperature, water_density);
    liquid_density = 1.0 / (xco2l / _co2_property.partialDensity(temperature) +
      (1.0 - xco2l) / water_density);
  }

  // The density of the gas phase, and the mass fraction of CO2 in the gase phase. If the
  // saturation is zero, set the phase density to zero (keep viscosity non-zero as it appears
  // in the denominator of the mobility.)
  Real gas_density, xco2g, co2_viscosity, vapour_viscosity;

  if (saturations[1] == 0.)
  {
    gas_density = 0.;
    xco2g = 0.;
    co2_viscosity = 1.;  // Not used
    vapour_viscosity = 1.;  // Not used
  }
  else
  {
    Real co2_density = _co2_property.density(co2_partial_pressure, temperature);
    Real vapour_density = _water_property.density(pv, temperature);
    gas_density = co2_density + vapour_density;
    xco2g = co2_density / gas_density;
    co2_viscosity = _co2_property.viscosity(co2_partial_pressure, temperature);
    vapour_viscosity = _water_property.viscosity(temperature, vapour_density);
  }

  // Now update the vector of thermophysical properties.
  // Liquid phase
  fluid_properties[0].push_back(liquid_density);
  fluid_properties[0].push_back(water_viscosity); // No effect due to dissolved co2
  fluid_properties[0].push_back(1.0 - xco2l); // H2o mass fraction in liquid
  fluid_properties[0].push_back(xco2l);

  // Gas phsae
  fluid_properties[1].push_back(gas_density);
  fluid_properties[1].push_back(co2_viscosity); // No effect due to vapour
  fluid_properties[1].push_back(1.0 - xco2g); // H2o mass fraction in gas
  fluid_properties[1].push_back(xco2g);

  return fluid_properties;
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
FluidStateWaterCO2::viscosity(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real fluid_viscosity;

  // TODO: Fix this so that density isn't calculated twice.
  if (phase_index == 0)
  {
    Real water_density = _water_property.density(pressure, temperature);
    fluid_viscosity = _water_property.viscosity(temperature, water_density);
  }
  else if (phase_index == 1)
    fluid_viscosity = _co2_property.viscosity(pressure, temperature);

  else
    mooseError("phase_index is out of range in FluidStateWaterCO2::viscosity");

  // TODO: effect of co2 in water on viscosity

  return fluid_viscosity;
}

std::vector<std::vector<Real> >
FluidStateWaterCO2::massFractions(Real pressure, Real temperature) const
{
  std::vector<std::vector<Real> > xmass;
  unsigned int numcomp = numComponents();
  unsigned int numphase = numPhases();

  xmass.resize(numcomp);

  // Calculate the dissolved mole fraction of CO2 in water
  Real xco2lm = pressure / FluidStateWaterCO2::henry(temperature);
  // The mass fraction is then
  Real xco2l = xco2lm * _Mco2 / (xco2lm * _Mco2 + (1.0 - xco2lm) * _Mh2o);

  xmass[0].push_back(1.0 - xco2l); // H2O in liquid
  xmass[0].push_back(0.0); // H2O in gas
  xmass[1].push_back(xco2l); // CO2 in liquid
  xmass[1].push_back(1.0); // CO2 in gas

  return xmass;
}

std::vector<Real>
FluidStateWaterCO2::relativePermeability(Real liquid_saturation) const
{
  std::vector<Real> relperm;

  relperm.push_back(_relative_permeability.relativePermLiq(liquid_saturation));
  relperm.push_back(_relative_permeability.relativePermGas(liquid_saturation));

  return relperm;
}

std::vector<Real>
FluidStateWaterCO2::pressure(Real gas_pressure, Real liquid_saturation) const
{
  std::vector<Real> pressures;
  Real capillary_pressure = _capillary_pressure.capillaryPressure(liquid_saturation);

  pressures.push_back(gas_pressure);
  pressures.push_back(gas_pressure - capillary_pressure);

  return pressures;
}

std::vector<Real>
FluidStateWaterCO2::dCapillaryPressure(Real liquid_saturation) const
{
  std::vector<Real> dpc;

  dpc.push_back(0.);
  dpc.push_back(_capillary_pressure.dCapillaryPressure(liquid_saturation));

  return dpc;
}

std::vector<Real>
FluidStateWaterCO2::saturation(Real liquid_saturation) const
{
  std::vector<Real> saturations;

  saturations.push_back(liquid_saturation);
  saturations.push_back(1.0 - liquid_saturation);

  return saturations;
}

std::vector<Real>
FluidStateWaterCO2::dDensity_dP(Real pressure, Real temperature) const
{
  Real dwater_density = _water_property.dDensity_dP(pressure, temperature);
  Real dco2_density = _co2_property.dDensity_dP(pressure, temperature);

  // Assuming that the liquid density is water, and gas is CO2 - ie no CO2 in liquid, no vapour in gas
  std::vector<Real> ddensities;
  ddensities.push_back(dwater_density);
  ddensities.push_back(dco2_density);

  return ddensities;
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
