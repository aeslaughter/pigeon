/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
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
  params.addCoupledVar("pressure_variable",  "The primary pressure variable");
  params.addCoupledVar("temperature_variable", 100, "The primary temperature variable.");
  params.addCoupledVar("saturation_variable", 1.0, "The primary saturation variable");
  params.addParam<bool>("isothermal", false, "Is the simulations isothermal?");
  return params;
}

FluidStateBrineCO2::FluidStateBrineCO2(const std::string & name, InputParameters parameters) :
  FluidState(name, parameters),

  _brine_property(getUserObject<FluidPropertiesBrine>("brine_property_uo")),
  _co2_property(getUserObject<FluidPropertiesCO2>("co2_property_uo")),
  _relative_permeability(getUserObject<RelativePermeability>("relative_permeability_uo")),
  _capillary_pressure(getUserObject<CapillaryPressure>("capillary_pressure_uo")),
  _pressure(coupledNodalValue("pressure_variable")),
  _temperature(coupledNodalValue("temperature_variable")),
  _saturation(coupledNodalValue("saturation_variable")),
  _is_isothermal(getParam<bool>("isothermal"))

{
  //  _fluid_properties.resize(_subproblem.mesh().nNodes());
    _fsp.resize(_mesh.nNodes());
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

Real
FluidStateBrineCO2::temperature() const
{
  return _temperature[0];
}

std::vector<std::string>
FluidStateBrineCO2::variable_names() const
{
  std::vector<std::string> varnames;
  varnames.push_back("gas_pressure");
  varnames.push_back("liquid_saturation");

  return varnames;
}

std::vector<std::string>
FluidStateBrineCO2::variable_types() const
{
  std::vector<std::string> vartypes;
  vartypes.push_back("pressure");
  vartypes.push_back("saturation");

  return vartypes;
}

std::vector<unsigned int>
FluidStateBrineCO2::variable_phase() const
{
  std::vector<unsigned int> varphases;
  varphases.push_back(1);
  varphases.push_back(0);

  return varphases;
}

void
FluidStateBrineCO2::execute()
{
  // Current node
  unsigned int node = _current_node->id();

  // Assign the fluid properties
  // Pressure and saturation first
  _fsp[node].pressure = pressure(_pressure[_qp], _saturation[_qp]);
  _fsp[node].saturation = saturation(_saturation[_qp]);
/*
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
*/
}

Real
FluidStateBrineCO2::getPressure(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].pressure[phase_index];
}

Real
FluidStateBrineCO2::getSaturation(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].saturation[phase_index];
}

Real
FluidStateBrineCO2::getDensity(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].density[phase_index];
}

Real
FluidStateBrineCO2::getViscosity(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].viscosity[phase_index];
}

Real
FluidStateBrineCO2::getRelativePermeability(unsigned int node_num, unsigned int phase_index) const
{
  return _fsp[node_num].relperm[phase_index];
}

Real
FluidStateBrineCO2::getMassFraction(unsigned int node_num, unsigned int phase_index, unsigned int component_index) const
{
  return _fsp[node_num].mass_fraction[component_index][phase_index];
}

/*
std::vector<std::vector<Real> >
FluidStateBrineCO2::thermophysicalProperties(Real pressure, Real temperature, Real saturation) const
{
  std::vector<std::vector<Real> > fluid_properties;

  return fluid_properties;
}
*/

Real
FluidStateBrineCO2::density(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real fluid_density;

  if (phase_index == 0)
  {
    Real xmass = 0.1; //FIX
    fluid_density = _brine_property.density(pressure, temperature, xmass);
  }
  else if (phase_index == 1)
    fluid_density = _co2_property.density(pressure, temperature);

  else
    mooseError("phase_index is out of range in FluidStateBrineCO2::density");

  return fluid_density;
}

Real
FluidStateBrineCO2::viscosity(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real xmass = 0.1; //FIX
  Real fluid_viscosity;

  if (phase_index == 0)
    fluid_viscosity = _brine_property.viscosity(pressure, temperature, xmass);

  else if (phase_index == 1)
    fluid_viscosity = _co2_property.viscosity(pressure, temperature);

  else
    mooseError("phase_index is out of range in FluidStateBrineCO2::viscosity");

  return fluid_viscosity;
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

std::vector<Real>
FluidStateBrineCO2::pressure(Real gas_pressure, Real liquid_saturation) const
{
  std::vector<Real> pressures;
  Real capillary_pressure = _capillary_pressure.capillaryPressure(liquid_saturation);

  pressures.push_back(gas_pressure - capillary_pressure);
  pressures.push_back(gas_pressure);

  return pressures;
}

std::vector<Real>
FluidStateBrineCO2::dCapillaryPressure(Real liquid_saturation) const
{
  std::vector<Real> dpc;

  dpc.push_back(0.);
  dpc.push_back(_capillary_pressure.dCapillaryPressure(liquid_saturation));

  return dpc;
}

std::vector<Real>
FluidStateBrineCO2::saturation(Real liquid_saturation) const
{
  std::vector<Real> saturations;

  saturations.push_back(liquid_saturation);
  saturations.push_back(1.0 - liquid_saturation);

  return saturations;
}

std::vector<Real>
FluidStateBrineCO2::dDensity_dP(Real pressure, Real temperature) const
{
  Real xmass = 0.1; //FIX

  Real dbrine_density = _brine_property.dDensity_dP(pressure, temperature, xmass);
  Real dco2_density = _co2_property.dDensity_dP(pressure, temperature);

  // Assuming that the liquid density is brine, and gas is CO2 - ie no CO2 in liquid, no vapour in gas
  std::vector<Real> ddensities;
  ddensities.push_back(dbrine_density);
  ddensities.push_back(dco2_density);

  return ddensities;
}

Real
FluidStateBrineCO2::henry(Real temperature) const
{
  // TODO: implement brine salting out correlation
  // Check temperature to make sure that it isn't out of the region of validity
  if (temperature < 1.04 || temperature > 369.51)
    mooseError("The temperature is out of range in FluidStateBrineCO2::henry");

  Real xmass = 0.1; //FIX THIS
  Real t_critical = 647.096;
  Real t_c2k = 273.15;
  std::vector<Real> a = _co2_property.henryConstants();
  Real tr = (temperature + t_c2k) / t_critical;

  Real kh = a[0] / tr + (a[1] / tr) * std::pow(1.0 - tr, 0.355) + a[2] *
            std::pow(tr, -0.41) * std::exp(1.0 - tr);

  return _brine_property.pSat(temperature, xmass) * std::exp(kh);
}
