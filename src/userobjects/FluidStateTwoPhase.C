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
  params.addParam<unsigned int>("pressure_variable_phase", 0, "The phase corresponding to the pressure variable");
  params.addParam<unsigned int>("saturation_variable_phase", 0, "The phase corresponding to the saturation variable");
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
  _not_isothermal(isCoupled("temperature_variable")),
  _p_phase(getParam<unsigned int>("pressure_variable_phase")),
  _s_phase(getParam<unsigned int>("saturation_variable_phase")),
  _pvar(coupled("pressure_variable")),
  _svar(coupled("saturation_variable")),
  _tvar(coupled("temperature_variable"))

{
  _fsp.resize(_mesh.nNodes());

  _num_components = 1;
  _num_phases = 2;
  _num_vars = _num_phases + _not_isothermal;

  _varnums.push_back(_pvar);
  if (_not_isothermal)
   _varnums.push_back(_tvar);

  // The pressure variable must always be coupled
  _pname = getVar("pressure_variable", 0)->name();
  // The saturation variable must always be coupled
  _sname = getVar("saturation_variable", 0)->name();
  if (_not_isothermal) // Check if temperature is coupled
    _tname = getVar("temperature_variable", 0)->name();
}

unsigned int
FluidStateTwoPhase::numPhases() const
{
  return _num_phases;
}

unsigned int
FluidStateTwoPhase::numComponents() const
{
  return _num_components;
}

bool
FluidStateTwoPhase::isIsothermal() const
{
   return 1 - _not_isothermal;  // Returns true if isothermal
}

Real
FluidStateTwoPhase::isothermalTemperature() const
{
  // For isothermal simulations
  return _temperature[0];
}

bool
FluidStateTwoPhase::isFluidStateVariable(unsigned int moose_var) const
{
  bool isvariable = true;
  if (std::find(_varnums.begin(), _varnums.end(), moose_var) == _varnums.end())
    isvariable = false;

  return isvariable;
}
std::string
FluidStateTwoPhase::variableNames(unsigned int moose_var) const
{
  std::string varname;

  if (moose_var == _pvar)
    varname = _pname;
  if (moose_var == _svar)
    varname = _sname;
  if (moose_var == _tvar)
    varname = _tname;

  return varname;
}

std::string
FluidStateTwoPhase::variableTypes(unsigned int moose_var) const
{
  std::string vartype;

  if (moose_var == _pvar)
    vartype = "pressure";
  if (moose_var == _svar)
    vartype = "saturation";
  if (moose_var == _tvar)
    vartype = "temperature";

  return vartype;
}

unsigned int
FluidStateTwoPhase::variablePhase(unsigned int moose_var) const
{
  unsigned int varphase;

  if (moose_var == _pvar)
    varphase = _p_phase;
  if (moose_var == _svar)
    varphase = _s_phase;

  return varphase;
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
