/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

/****************************************************************/
/* Fluid properties of brine.                                   */
/*                                                              */
/* Chris Green 2015                                             */
/* chris.green@csiro.au                                         */
/****************************************************************/

#include "FluidStateBrineCO2.h"

template<>
InputParameters validParams<FluidStateBrineCO2>()
{
  InputParameters params = validParams<FluidState>();
  params.addClassDescription("Thermophysical fluid state of brine (H20 - NaCl mixtures) and CO2.");
  params.addRequiredParam<UserObjectName>("brine_property_uo", "Name of the User Object defining the brine properties");
  params.addRequiredParam<UserObjectName>("co2_property_uo", "Name of the User Object defining the CO2 properties");
  params.addRequiredParam<UserObjectName>("relative_permeability_uo", "Name of the User Object defining the relative permeabilities");
  params.addRequiredParam<UserObjectName>("capillary_pressure_uo", "Name of the User Object defining the capillary pressure");
  params.addParam<Real>("fluid_temperature", 20.0, "Isothermal fluid temperature");
  params.addParam<bool>("isothermal", false, "Is the simulations isothermal?");
  return params;
}

FluidStateBrineCO2::FluidStateBrineCO2(const std::string & name, InputParameters parameters) :
  FluidState(name, parameters),

  _brine_property(getUserObject<FluidPropertiesBrine>("brine_property_uo")),
  _co2_property(getUserObject<FluidPropertiesCO2>("co2_property_uo")),
  _relative_permeability(getUserObject<RelativePermeability>("relative_permeability_uo")),
  _capillary_pressure(getUserObject<CapillaryPressure>("capillary_pressure_uo")),
  _fluid_temperature(getParam<Real>("fluid_temperature")),
  _is_isothermal(getParam<bool>("isothermal"))

{
}

unsigned int
  FluidStateBrineCO2::numPhases() const
{
  return 2;
}

unsigned int
  FluidStateBrineCO2::numComponents() const
{
  return 2;
}

bool
   FluidStateBrineCO2::isIsothermal() const
{
   return _is_isothermal;
}

Real FluidStateBrineCO2::temperature() const
{
  return _fluid_temperature;
}

std::vector<Real>
  FluidStateBrineCO2::density(Real pressure, Real temperature) const
{
  Real xmass = 0.1; //FIX
  Real brine_density = _brine_property.density(pressure, temperature, xmass);
  Real co2_density = _co2_property.density(pressure, temperature);

  std::vector<Real> densities;
  densities.push_back(brine_density);
  densities.push_back(co2_density);

  return densities;
}

std::vector<Real>
  FluidStateBrineCO2::viscosity(Real pressure, Real temperature) const
{
  Real xmass = 0.1; //FIX
  Real brine_viscosity = _brine_property.viscosity(pressure, temperature, xmass);
  Real co2_viscosity = _co2_property.viscosity(pressure, temperature);

  std::vector<Real> viscosities;
  viscosities.push_back(brine_viscosity);
  viscosities.push_back(co2_viscosity);

  return viscosities;
}

std::vector<std::vector<Real> >
   FluidStateBrineCO2::massFractions(Real pressure, Real temperature) const
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
   FluidStateBrineCO2::relativePermeability(Real liquid_saturation) const
{
  std::vector<Real> relperm;

  relperm.push_back(_relative_permeability.relativePermLiq(liquid_saturation));
  relperm.push_back(_relative_permeability.relativePermGas(liquid_saturation));

  return relperm;
}

Real
   FluidStateBrineCO2::pressure(Real gas_pressure, Real liquid_saturation) const
{
  Real capillary_pressure = _capillary_pressure.capillaryPressure(liquid_saturation);

  return gas_pressure - capillary_pressure;
}

Real
   FluidStateBrineCO2::saturation(Real liquid_saturation) const
{
  return 1.0 - liquid_saturation;
}

