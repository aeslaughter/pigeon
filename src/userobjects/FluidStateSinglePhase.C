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
  params.addCoupledVar("pressure_variable",  "The primary pressure variable");
  params.addCoupledVar("temperature_variable", 100, "The primary temperature variable.");
  params.addCoupledVar("saturation_variable", 1.0, "The primary saturation variable");
  params.addParam<Real>("fluid_temperature", 20.0, "Isothermal fluid temperature");
  params.addParam<unsigned int>("num_components", 1, "The number of components in the fluid");
  params.addParam<bool>("isothermal", false, "Is the simulations isothermal?");
  return params;
}

FluidStateSinglePhase::FluidStateSinglePhase(const std::string & name, InputParameters parameters) :
  FluidState(name, parameters),

  _fluid_property(getUserObject<FluidPropertiesWater>("fluid_property_uo")),
  _pressure(coupledNodalValue("pressure_variable")),
  _temperature(coupledNodalValue("temperature_variable")),
  _saturation(coupledNodalValue("saturation_variable")),
  _fluid_temperature(getParam<Real>("fluid_temperature")),
  _num_components(getParam<unsigned int>("num_components")),
  _is_isothermal(getParam<bool>("isothermal"))

{
  //  _fluid_properties.resize(_subproblem.mesh().nNodes());
    _fsp.resize(_mesh.nNodes());
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

std::vector<std::string>
FluidStateSinglePhase::variable_names() const
{
  std::vector<std::string> varnames;
  varnames.push_back("pressure");

  return varnames;
}


std::vector<std::string>
FluidStateSinglePhase::variable_types() const
{
  std::vector<std::string> vartypes;
  vartypes.push_back("pressure");

  return vartypes;
}

std::vector<unsigned int>
FluidStateSinglePhase::variable_phase() const
{
  std::vector<unsigned int> varphases;
  varphases.push_back(0);

  return varphases;
}

void
FluidStateSinglePhase::execute()
{
  // Current node
  unsigned int node = _current_node->id();

  // Assign the fluid properties
  // Pressure and saturation first
  _fsp[node].pressure = pressure(_pressure[_qp], _saturation[_qp]);
  _fsp[node].saturation = saturation(_saturation[_qp]);

  // Density of fluid
  std::vector<Real> densities;

  densities.push_back(_fluid_property.density(_fsp[node].pressure[0], _temperature[_qp]));
  _fsp[node].density = densities;

  // Viscosity of fluid
  std::vector<Real> viscosities;
  // Water viscosity uses water density in calculation. TODO Fix this for generality
  viscosities.push_back(_fluid_property.viscosity(_temperature[_qp], _fsp[node].density[0]));
  _fsp[node].viscosity = viscosities;

  // Relative permeability of the phase is equal to 1.0 by definition
  _fsp[node].relperm = relativePermeability(_fsp[node].saturation[0]);

  // Mass fraction of each component in the fluid
  std::vector<std::vector<Real> > xmass;
  xmass.resize(numComponents());

  xmass[0].push_back(1.0); // Only component in liquid

  _fsp[node].mass_fraction = xmass;

}

Real
FluidStateSinglePhase::getPressure(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].pressure[phase_index];
}

Real
FluidStateSinglePhase::getSaturation(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].saturation[phase_index];
}

Real
FluidStateSinglePhase::getDensity(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].density[phase_index];
}

Real
FluidStateSinglePhase::getViscosity(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].viscosity[phase_index];
}

Real
FluidStateSinglePhase::getRelativePermeability(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].relperm[phase_index];
}

Real
FluidStateSinglePhase::getMassFraction(unsigned int node_num, unsigned int phase_index, unsigned int component_index) const
{
  return _fsp[node_num].mass_fraction[component_index][phase_index];
}

Real
FluidStateSinglePhase::density(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real fluid_density = _fluid_property.density(pressure, temperature);

  return fluid_density;
}

Real
FluidStateSinglePhase::viscosity(Real pressure, Real temperature, unsigned int phase_index) const
{
  // TODO: Fix this so that density isn't calculated twice.
  Real fluid_density = _fluid_property.density(pressure, temperature);
  Real fluid_viscosity = _fluid_property.viscosity(temperature, fluid_density);

  return fluid_viscosity;
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

  dpc.push_back(0.);

  return dpc;
}

std::vector<Real>
FluidStateSinglePhase::saturation(Real liquid_saturation) const
{
  std::vector<Real> saturations;

  saturations.push_back(liquid_saturation);

  return saturations;
}

std::vector<Real>
FluidStateSinglePhase::dDensity_dP(Real pressure, Real temperature) const
{
  Real dfluid_density = _fluid_property.dDensity_dP(pressure, temperature);

  std::vector<Real> ddensities;
  ddensities.push_back(dfluid_density);

  return ddensities;
}
