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
  params.addRequiredCoupledVar("temperature_variable", "The fluid temperature variable. For isothermal simulations, enter the fluid temperature (C)");
  params.addRequiredCoupledVar("pressure_variable", "The fluid pressure variable (Pa)");
  return params;
}

FluidStateSinglePhase::FluidStateSinglePhase(const std::string & name, InputParameters parameters) :
  FluidState(name, parameters),

  _fluid_property(getUserObject<FluidProperties>("fluid_property_uo")),
  _pressure(coupledNodalValue("pressure_variable")),
  _temperature(coupledNodalValue("temperature_variable")),
  _not_isothermal(isCoupled("temperature_variable")),
  _pvar(coupled("pressure_variable")),
  _tvar(coupled("temperature_variable"))

{
  _fsp.resize(_mesh.nNodes());

  _num_components = 1;
  _num_phases = 1;
  _num_vars = _num_phases + _num_components + _not_isothermal;

  _varnums.push_back(_pvar);
  if (_not_isothermal)
   _varnums.push_back(_tvar);

  // The pressure variable must always be coupled
  _pname = getVar("pressure_variable", 0)->name();
  if (_not_isothermal) // Check if temperature is coupled
    _tname = getVar("temperature_variable", 0)->name();
}

unsigned int
FluidStateSinglePhase::numPhases() const
{
  return _num_phases;
}

unsigned int
FluidStateSinglePhase::numComponents() const
{
  return _num_components;
}

bool
FluidStateSinglePhase::isIsothermal() const
{
   return 1 - _not_isothermal;  // Returns true if isothermal
}

Real
FluidStateSinglePhase::isothermalTemperature() const
{
  // For isothermal simulations
  return _temperature[_qp];
}

unsigned int
FluidStateSinglePhase::temperatureVar() const
{
  return _tvar;
}

bool
FluidStateSinglePhase::isFluidStateVariable(unsigned int moose_var) const
{
  bool isvariable = true;
  if (std::find(_varnums.begin(), _varnums.end(), moose_var) == _varnums.end())
    isvariable = false;

  return isvariable;
}


std::string
FluidStateSinglePhase::variableNames(unsigned int moose_var) const
{
  std::string varname;
  if (moose_var == _pvar)
    varname = _pname;
  if (moose_var == _tvar)
    varname = _tname;

  return varname;
}

std::string
FluidStateSinglePhase::variableTypes(unsigned int moose_var) const
{
  std::string vartype;
  if (moose_var == _pvar)
    vartype = "pressure";
  if (moose_var == _tvar)
    vartype = "temperature";

  return vartype;
}

unsigned int
FluidStateSinglePhase::variablePhase(unsigned int moose_var) const
{
 // Only one phase in this FluidState
  return 0;
}

void
FluidStateSinglePhase::execute()
{
  // Current node
  unsigned int node = _current_node->id();
  Real fluid_saturation = 1.0;

  // Store the phase pressures
  _fsp[node].pressure = pressure(_pressure[_qp], fluid_saturation);

  // Store the phase saturations
  _fsp[node].saturation = saturation(fluid_saturation);

  // Density of phases
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
