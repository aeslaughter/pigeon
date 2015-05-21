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
  params.addClassDescription("Thermophysical fluid state of water (H20) and CO2.");
  params.addRequiredParam<UserObjectName>("fluid_property_uo", "Name of the User Object defining the water properties");
  params.addParam<Real>("fluid_temperature", 20.0, "Isothermal fluid temperature");
  params.addParam<unsigned int>("num_components", 1, "The number of components in the fluid");
  params.addParam<bool>("isothermal", false, "Is the simulations isothermal?");
  return params;
}

FluidStateSinglePhase::FluidStateSinglePhase(const std::string & name, InputParameters parameters) :
  FluidState(name, parameters),

  _fluid_property(getUserObject<FluidPropertiesWater>("fluid_property_uo")),
  _fluid_temperature(getParam<Real>("fluid_temperature")),
  _num_components(getParam<unsigned int>("num_components")),
  _is_isothermal(getParam<bool>("isothermal"))

{
}

unsigned int
  FluidStateSinglePhase::numPhases() const
{
  return 1;
}

unsigned int
  FluidStateSinglePhase::numComponents() const
{
  return _num_components;
}

bool
   FluidStateSinglePhase::isIsothermal() const
{
   return _is_isothermal;
}

Real
   FluidStateSinglePhase::temperature() const
{
  return _fluid_temperature;
}

std::vector<Real>
  FluidStateSinglePhase::density(Real pressure, Real temperature) const
{
  Real fluid_density = _fluid_property.density(pressure, temperature);

  std::vector<Real> densities;
  densities.push_back(fluid_density);

  return densities;
}

std::vector<Real>
  FluidStateSinglePhase::viscosity(Real pressure, Real temperature) const
{
  // TODO: Fix this so that density isn't calculated twice.
  Real fluid_density = _fluid_property.density(pressure, temperature);
  Real fluid_viscosity = _fluid_property.viscosity(temperature, fluid_density);

  std::vector<Real> viscosities;
  viscosities.push_back(fluid_viscosity);

  return viscosities;
}

std::vector<std::vector<Real> >
   FluidStateSinglePhase::massFractions(Real pressure, Real temperature) const
{
  std::vector<std::vector<Real> > xmass;
  unsigned int numcomp = numComponents();
  unsigned int numphase = numPhases();

  xmass.resize(numcomp);

  xmass[0].push_back(1.0); // Component in phase
  xmass[0].push_back(0.0); // No other phase

  return xmass;
}

std::vector<Real>
   FluidStateSinglePhase::relativePermeability(Real liquid_saturation) const
{
  std::vector<Real> relperm;

  relperm.push_back(1.0);

  return relperm;
}

Real
   FluidStateSinglePhase::pressure(Real gas_pressure, Real liquid_saturation) const
{
  return 0.;
}

Real
   FluidStateSinglePhase::saturation(Real liquid_saturation) const
{
  return 1.0;
}
