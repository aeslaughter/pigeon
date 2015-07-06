/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidStateMultiphase.h"

template<>
InputParameters validParams<FluidStateMultiphase>()
{
  InputParameters params = validParams<FluidState>();
  params.addClassDescription("Thermophysical fluid state of liquid and gas.");
  params.addRequiredParam<UserObjectName>("liquid_property_uo", "Name of the User Object defining the liquid properties");
  params.addRequiredParam<UserObjectName>("gas_property_uo", "Name of the User Object defining the gas properties");
  params.addRequiredParam<UserObjectName>("relative_permeability_uo", "Name of the User Object defining the relative permeabilities");
  params.addRequiredParam<UserObjectName>("capillary_pressure_uo", "Name of the User Object defining the capillary pressure");
  params.addParam<Real>("fluid_temperature", 20.0, "Isothermal fluid temperature");
  params.addParam<bool>("isothermal", false, "Is the simulations isothermal?");
  return params;
}

FluidStateMultiphase::FluidStateMultiphase(const std::string & name, InputParameters parameters) :
  FluidState(name, parameters),

  _liquid_property(getUserObject<FluidProperties>("liquid_property_uo")),
  _gas_property(getUserObject<FluidProperties>("gas_property_uo")),
  _relative_permeability(getUserObject<RelativePermeability>("relative_permeability_uo")),
  _capillary_pressure(getUserObject<CapillaryPressure>("capillary_pressure_uo")),
  _fluid_temperature(getParam<Real>("fluid_temperature")),
  _is_isothermal(getParam<bool>("isothermal"))

{
}

unsigned int
FluidStateMultiphase::numPhases() const
{
  return 2;
}

unsigned int
FluidStateMultiphase::numComponents() const
{
  return 2;
}

bool
FluidStateMultiphase::isIsothermal() const
{
   return _is_isothermal;
}

Real
FluidStateMultiphase::temperature() const
{
  return _fluid_temperature;
}

std::vector<std::string>
FluidStateMultiphase::variable_names() const
{
  std::vector<std::string> varnames;
  varnames.push_back("gas_pressure");
  varnames.push_back("liquid_saturation");

  return varnames;
}

std::vector<std::string>
FluidStateMultiphase::variable_types() const
{
  std::vector<std::string> varnames;
  varnames.push_back("pressure");
  varnames.push_back("saturation");

  return varnames;
}

std::vector<unsigned int>
FluidStateMultiphase::variable_phase() const
{
  std::vector<unsigned int> varphases;
  varphases.push_back(1);  // gas phase
  varphases.push_back(0);  // liquid phase

  return varphases;
}

std::vector<std::vector<Real> >
FluidStateMultiphase::thermophysicalProperties(Real pressure, Real temperature, Real saturation) const
{
  //_console << "thermophysicalProperties called" << std::endl;
  std::vector<std::vector<Real> > fluid_properties;
  fluid_properties.resize(numPhases());

  // The pressures of the liquid and fluid phases
  std::vector<Real> fluid_pressures;
  fluid_pressures = FluidStateMultiphase::pressure(pressure, saturation);

  // Update the vector of thermophysical properties.
  // Liquid phase
  fluid_properties[0].push_back(density(fluid_pressures[0], temperature, 0));
  fluid_properties[0].push_back(viscosity(fluid_pressures[0], temperature, 0)); // No effect due to dissolved co2
  fluid_properties[0].push_back(1.0); // H2o mass fraction in liquid
  fluid_properties[0].push_back(0.);

  // Gas phsae
  fluid_properties[1].push_back(density(fluid_pressures[1], temperature, 1));
  fluid_properties[1].push_back(viscosity(fluid_pressures[1], temperature, 1)); // No effect due to vapour
  fluid_properties[1].push_back(0.); // H2o mass fraction in gas
  fluid_properties[1].push_back(1.);

  return fluid_properties;
}

Real
FluidStateMultiphase::density(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real fluid_density;

  if (phase_index == 0) // Liquid phase
    fluid_density = _liquid_property.density(pressure, temperature);

  else if (phase_index == 1) // Gas phase
    fluid_density = _gas_property.density(pressure, temperature);

  else
    mooseError("phase_index is out of range in FluidStateMultiphase::density");

  return fluid_density;
}

Real
FluidStateMultiphase::viscosity(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real fluid_viscosity;

  // TODO: Fix this so that density isn't calculated twice.
  if (phase_index == 0)
  {
    Real liquid_density = _liquid_property.density(pressure, temperature);
    fluid_viscosity = _liquid_property.viscosity(temperature, liquid_density);
  }
  else if (phase_index == 1)
    fluid_viscosity = _gas_property.viscosity(pressure, temperature);

  else
    mooseError("phase_index is out of range in FluidStateWaterCO2::viscosity");

  // TODO: effect of co2 in water on viscosity

  return fluid_viscosity;
}

std::vector<std::vector<Real> >
FluidStateMultiphase::massFractions(Real pressure, Real temperature) const
{
  std::vector<std::vector<Real> > xmass;
  unsigned int numcomp = numComponents();
  unsigned int numphase = numPhases();

  xmass.resize(numcomp);

  xmass[0].push_back(1.0); // Only liquid component in liquid
  xmass[0].push_back(0.0); // No liquid component in gas
  xmass[1].push_back(0.0); // No gas component in liquid
  xmass[1].push_back(1.0); // Only gas component in gas

  return xmass;
}

std::vector<Real>
FluidStateMultiphase::relativePermeability(Real liquid_saturation) const
{
  std::vector<Real> relperm;

  relperm.push_back(_relative_permeability.relativePermLiq(liquid_saturation));
  relperm.push_back(_relative_permeability.relativePermGas(liquid_saturation));

  return relperm;
}

std::vector<Real>
FluidStateMultiphase::pressure(Real gas_pressure, Real liquid_saturation) const
{
  std::vector<Real> pressures;
  Real capillary_pressure = _capillary_pressure.capillaryPressure(liquid_saturation);

  pressures.push_back(gas_pressure + capillary_pressure);
  pressures.push_back(gas_pressure);

  return pressures;
}

std::vector<Real>
FluidStateMultiphase::dCapillaryPressure(Real liquid_saturation) const
{
  std::vector<Real> dpc;

  dpc.push_back(0.);
  dpc.push_back(_capillary_pressure.dCapillaryPressure(liquid_saturation));

  return dpc;
}

std::vector<Real>
FluidStateMultiphase::saturation(Real liquid_saturation) const
{
  std::vector<Real> saturations;

  saturations.push_back(liquid_saturation);
  saturations.push_back(1.0 - liquid_saturation);

  return saturations;
}

std::vector<Real>
FluidStateMultiphase::dDensity_dP(Real pressure, Real temperature) const
{
  Real dliquid_density = _liquid_property.dDensity_dP(pressure, temperature);
  Real dgas_density = _gas_property.dDensity_dP(pressure, temperature);

  std::vector<Real> ddensities;
  ddensities.push_back(dliquid_density);
  ddensities.push_back(dgas_density);

  return ddensities;
}

Real
FluidStateMultiphase::henry(Real temperature) const
{
  return 0.;
}

Real
FluidStateMultiphase::dissolved(Real pressure, Real temperature) const
{
  return 0.;
}
