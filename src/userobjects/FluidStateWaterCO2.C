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
  varphases.push_back(1);
  varphases.push_back(0);

  return varphases;
}

std::vector<Real>
  FluidStateWaterCO2::density(Real pressure, Real temperature) const
{
  Real water_density = _water_property.density(pressure, temperature);
  Real co2_density = _co2_property.density(pressure, temperature);

  // Assuming that the liquid density is water, and gas is CO2 - ie no CO2 in liquid, no vapour in gas
  std::vector<Real> densities;
  densities.push_back(water_density);
  densities.push_back(co2_density);

  return densities;
}

std::vector<Real>
  FluidStateWaterCO2::viscosity(Real pressure, Real temperature) const
{
  // TODO: Fix this so that density isn't calculated twice.
  Real water_density = _water_property.density(pressure, temperature);
  Real water_viscosity = _water_property.viscosity(temperature, water_density);
  Real co2_viscosity = _co2_property.viscosity(pressure, temperature);

  // Assuming that the liquid viscosity is water, and gas is CO2 - ie no CO2 in liquid, no vapour in gas
  std::vector<Real> viscosities;
  viscosities.push_back(water_viscosity);
  viscosities.push_back(co2_viscosity);

  return viscosities;
}

std::vector<std::vector<Real> >
   FluidStateWaterCO2::massFractions(Real pressure, Real temperature) const
{
  std::vector<std::vector<Real> > xmass;
  unsigned int numcomp = numComponents();
  unsigned int numphase = numPhases();

  xmass.resize(numcomp);

  xmass[0].push_back(1.0); // H2O in liquid
  xmass[0].push_back(0.0); // H2O in gas
  xmass[1].push_back(0.0); // CO2 in liquid
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
  Real t_critical = 647.096;
  Real t_c2k = 273.15;
  std::vector<Real> a = _co2_property.henryConstants();
  Real tr = (temperature + t_c2k) / t_critical;

  Real kh = a[0] / tr + (a[1] / tr) * std::pow(1.0 - tr, 0.355) + a[2] *
            std::pow(tr, -0.41) * std::exp(1.0 - tr);

_console << "a " << a[1] << " " << a[2]<< std::endl;
  return _water_property.pSat(temperature) * std::exp(kh);
}
