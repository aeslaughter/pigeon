/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "FluidStateTwoPhase.h"

template<>
InputParameters validParams<FluidStateTwoPhase>()
{
  InputParameters params = validParams<FluidState>();
  params.addClassDescription("Thermophysical fluid state of liquid and gas.");
  params.addRequiredParam<UserObjectName>("liquid_property_uo", "Name of the User Object defining the liquid properties");
  params.addRequiredParam<UserObjectName>("gas_property_uo", "Name of the User Object defining the gas properties");
  params.addRequiredParam<UserObjectName>("relative_permeability_uo", "Name of the User Object defining the relative permeabilities");
  params.addRequiredParam<UserObjectName>("capillary_pressure_uo", "Name of the User Object defining the capillary pressure");
  params.addCoupledVar("pressure_variable",  "The primary pressure variable");
  params.addCoupledVar("temperature_variable", 100, "The primary temperature variable.");
  params.addCoupledVar("saturation_variable", 1.0, "The primary saturation variable");
  params.addParam<Real>("fluid_temperature", 20.0, "Isothermal fluid temperature");
  params.addParam<bool>("isothermal", false, "Is the simulations isothermal?");
  return params;
}

FluidStateTwoPhase::FluidStateTwoPhase(const std::string & name, InputParameters parameters) :
  FluidState(name, parameters),

  _liquid_property(getUserObject<FluidProperties>("liquid_property_uo")),
  _gas_property(getUserObject<FluidProperties>("gas_property_uo")),
  _relative_permeability(getUserObject<RelativePermeability>("relative_permeability_uo")),
  _capillary_pressure(getUserObject<CapillaryPressure>("capillary_pressure_uo")),
  _pressure(coupledNodalValue("pressure_variable")),
  _temperature(coupledNodalValue("temperature_variable")),
  _saturation(coupledNodalValue("saturation_variable")),
  _fluid_temperature(getParam<Real>("fluid_temperature")),
  _is_isothermal(getParam<bool>("isothermal"))

{
//  _fluid_properties.resize(_subproblem.mesh().nNodes());
  _fsp.resize(_mesh.nNodes());
}

unsigned int
FluidStateTwoPhase::numPhases() const
{
  return 2;
}

unsigned int
FluidStateTwoPhase::numComponents() const
{
  return 2;
}

bool
FluidStateTwoPhase::isIsothermal() const
{
   return _is_isothermal;
}

Real
FluidStateTwoPhase::temperature() const
{
  return _fluid_temperature;
}

std::vector<std::string>
FluidStateTwoPhase::variable_names() const
{
  std::vector<std::string> varnames;
  varnames.push_back("gas_pressure");
  varnames.push_back("liquid_saturation");

  return varnames;
}

std::vector<std::string>
FluidStateTwoPhase::variable_types() const
{
  std::vector<std::string> varnames;
  varnames.push_back("pressure");
  varnames.push_back("saturation");

  return varnames;
}

std::vector<unsigned int>
FluidStateTwoPhase::variable_phase() const
{
  std::vector<unsigned int> varphases;
  varphases.push_back(1);  // gas phase
  varphases.push_back(0);  // liquid phase

  return varphases;
}

void
FluidStateTwoPhase::execute()
{
  // Current node
  unsigned int node = _current_node->id();

  // Assign the fluid properties
  // Pressure and saturation first
  _fsp[node].pressure = pressure(_pressure[_qp], _saturation[_qp]);
  _fsp[node].saturation = saturation(_saturation[_qp]);

  // Density of each phase
  std::vector<Real> densities;

  densities.push_back(_liquid_property.density(_fsp[node].pressure[0], _temperature[_qp]));
  densities.push_back(_gas_property.density(_fsp[node].pressure[1], _temperature[_qp]));
  _fsp[node].density = densities;

  // Viscosity of each phase
  std::vector<Real> viscosities;
  // Water viscosity uses water density in calculation.
  viscosities.push_back(_liquid_property.viscosity(_temperature[_qp], _fsp[node].density[0]));
  viscosities.push_back(_gas_property.viscosity(_fsp[node].pressure[1], _temperature[_qp]));
  _fsp[node].viscosity = viscosities;

  // Relative permeability of each phase
  _fsp[node].relperm = relativePermeability(_fsp[node].saturation[0]);

  // Mass fraction of each component in each phase
  std::vector<std::vector<Real> > xmass;
  xmass.resize(numComponents());

  xmass[0].push_back(1.0); // Only liquid component in liquid
  xmass[0].push_back(0.0); // No liquid component in gas
  xmass[1].push_back(0.0); // No gas component in liquid
  xmass[1].push_back(1.0); // Only gas component in gas

  _fsp[node].mass_fraction = xmass;

}

Real
FluidStateTwoPhase::getPressure(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].pressure[phase_index];
}

Real
FluidStateTwoPhase::getSaturation(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].saturation[phase_index];
}

Real
FluidStateTwoPhase::getDensity(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].density[phase_index];
}

Real
FluidStateTwoPhase::getViscosity(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].viscosity[phase_index];
}

Real
FluidStateTwoPhase::getRelativePermeability(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].relperm[phase_index];
}

Real
FluidStateTwoPhase::getMassFraction(unsigned int node_num, unsigned int phase_index, unsigned int component_index) const
{
  return _fsp[node_num].mass_fraction[component_index][phase_index];
}

Real
FluidStateTwoPhase::density(Real pressure, Real temperature, unsigned int phase_index) const
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
FluidStateTwoPhase::viscosity(Real pressure, Real temperature, unsigned int phase_index) const
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
FluidStateTwoPhase::massFractions(Real pressure, Real temperature) const
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
FluidStateTwoPhase::relativePermeability(Real liquid_saturation) const
{
  std::vector<Real> relperm;

  relperm.push_back(_relative_permeability.relativePermLiq(liquid_saturation));
  relperm.push_back(_relative_permeability.relativePermGas(liquid_saturation));

  return relperm;
}

std::vector<Real>
FluidStateTwoPhase::pressure(Real gas_pressure, Real liquid_saturation) const
{
  std::vector<Real> pressures;
  Real capillary_pressure = _capillary_pressure.capillaryPressure(liquid_saturation);

  pressures.push_back(gas_pressure + capillary_pressure);
  pressures.push_back(gas_pressure);

  return pressures;
}

std::vector<Real>
FluidStateTwoPhase::dCapillaryPressure(Real liquid_saturation) const
{
  std::vector<Real> dpc;

  dpc.push_back(0.);
  dpc.push_back(_capillary_pressure.dCapillaryPressure(liquid_saturation));

  return dpc;
}

std::vector<Real>
FluidStateTwoPhase::saturation(Real liquid_saturation) const
{
  std::vector<Real> saturations;

  saturations.push_back(liquid_saturation);
  saturations.push_back(1.0 - liquid_saturation);

  return saturations;
}

std::vector<Real>
FluidStateTwoPhase::dDensity_dP(Real pressure, Real temperature) const
{
  Real dliquid_density = _liquid_property.dDensity_dP(pressure, temperature);
  Real dgas_density = _gas_property.dDensity_dP(pressure, temperature);

  std::vector<Real> ddensities;
  ddensities.push_back(dliquid_density);
  ddensities.push_back(dgas_density);

  return ddensities;
}

Real
FluidStateTwoPhase::henry(Real temperature) const
{
  return 0.;
}

Real
FluidStateTwoPhase::dissolved(Real pressure, Real temperature) const
{
  return 0.;
}
