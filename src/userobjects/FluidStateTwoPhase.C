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
  params.addClassDescription("Thermophysical fluid state of two-phase sliquid and gas.");
  params.addRequiredParam<UserObjectName>("liquid_property_uo", "Name of the User Object defining the liquid properties");
  params.addRequiredParam<UserObjectName>("gas_property_uo", "Name of the User Object defining the gas properties");
  params.addRequiredParam<UserObjectName>("relative_permeability_uo", "Name of the User Object defining the relative permeabilities");
  params.addRequiredParam<UserObjectName>("capillary_pressure_uo", "Name of the User Object defining the capillary pressure");
  params.addRequiredCoupledVar("pressure_variable",  "The primary pressure variable");
  params.addRequiredCoupledVar("temperature_variable", "The primary temperature variable.");
  params.addRequiredCoupledVar("saturation_variable", "The primary saturation variable");
  params.addParam<unsigned int>("pressure_variable_phase", 0, "The phase corresponding to the pressure variable");
  params.addParam<unsigned int>("saturation_variable_phase", 0, "The phase corresponding to the saturation variable");
  params.addParam<unsigned int>("component_index", 1, "The index of the primary mass fraction component");
  return params;
}

FluidStateTwoPhase::FluidStateTwoPhase(const InputParameters & parameters) :
  FluidState(parameters),

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
  _tvar(coupled("temperature_variable")),
  _component_index(getParam<unsigned int>("component_index"))

{
  _num_components = 2;
  _num_phases = 2;
  _num_vars = _coupled_moose_vars.size() + (1 - _not_isothermal);

  _varnums.push_back(_pvar);
  _varnums.push_back(_svar);
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
  return _temperature[_qp];
}

unsigned int
FluidStateTwoPhase::temperatureVar() const
{
  return _tvar;
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
  unsigned int varphase = 0;

  if (moose_var == _pvar)
    varphase = _p_phase;
  if (moose_var == _svar)
    varphase = _s_phase;

  return varphase;
}

unsigned int
FluidStateTwoPhase::primaryComponentIndex() const
{
  return _component_index;
}

void
FluidStateTwoPhase::initialize()
{
  _nodal_properties.clear();
  _primary_vars.resize(_num_vars);
}

void
FluidStateTwoPhase::execute()
{
  MooseVariable * pvar, * svar, * tvar;
  FluidStateProperties nodalfsp;

  if (isCoupled("pressure_variable"))
    pvar = getVar("pressure_variable", 0);
  if (isCoupled("saturation_variable"))
    svar = getVar("saturation_variable", 0);
  if(isCoupled("temperature_variable"))
    tvar = getVar("temperature_variable", 0);

  // Loop over all elements on current processor
  const MeshBase::element_iterator end = _mesh.getMesh().active_local_elements_end();
  for (MeshBase::element_iterator el = _mesh.getMesh().active_local_elements_begin(); el != end; ++el)
  {
    const Elem * current_elem = *el;

    // Loop over all nodes on each element
    for (unsigned int i = 0; i < current_elem->n_vertices(); ++i)
    {
      const Node * current_node = current_elem->get_node(i);
      unsigned int nodeid = current_node->id();

      // Check if the properties at this node have already been calculated, and if so,
      // skip to the next node
      if (_nodal_properties.find(nodeid) == _nodal_properties.end())
      {
        if (isCoupled("pressure_variable"))
          _primary_vars[0] = pvar->getNodalValue(*current_node);
        else
          _primary_vars[0] = _pressure[_qp];

        if (isCoupled("saturation_variable"))
          _primary_vars[1] = svar->getNodalValue(*current_node);
        else
          _primary_vars[1] = _saturation[_qp];

        if (isCoupled("temperature_variable"))
          _primary_vars[2] = tvar->getNodalValue(*current_node);
        else
          _primary_vars[2] = _temperature[_qp];

        // Now calculate all thermophysical properties at the current node
        thermophysicalProperties(_primary_vars, nodalfsp);

        // Now insert these properties into the _nodal_properties map
        _nodal_properties.insert( std::pair<int, FluidStateProperties>(nodeid, nodalfsp));
      }
    }
  }
}

void
FluidStateTwoPhase::thermophysicalProperties(std::vector<Real> primary_vars, FluidStateProperties & fsp)
{
  // Primary variables at the node
  Real node_pressure = primary_vars[0];
  Real node_saturation = primary_vars[1];
  Real node_temperature = primary_vars[2];

  // Assign the fluid properties
  // Saturation
  fsp.saturation = saturation(node_saturation);

  // Pressure (takes liquid saturation for capillary pressure calculation)
  fsp.pressure = pressure(node_pressure, fsp.saturation[0]);

  // Density of each phase
  std::vector<Real> densities(_num_phases);

  densities[0] = _liquid_property.density(fsp.pressure[0], node_temperature);
  densities[1] = _gas_property.density(fsp.pressure[1], node_temperature);

  fsp.density = densities;

  // Viscosity of each phase
  std::vector<Real> viscosities(_num_phases);

  // Water viscosity uses water density in calculation.
  viscosities[0] = _liquid_property.viscosity(node_temperature, fsp.density[0]);
  viscosities[1] = _gas_property.viscosity(fsp.pressure[1], node_temperature);

  fsp.viscosity = viscosities;

  // Relative permeability of each phase
  fsp.relperm = relativePermeability(fsp.saturation[0]);

  // Mass fraction of each component in each phase
  std::vector<std::vector<Real> > xmass;
  xmass.resize(numComponents());

  xmass[0].push_back(1.0); // Only liquid component in liquid
  xmass[0].push_back(0.0); // No liquid component in gas
  xmass[1].push_back(0.0); // No gas component in liquid
  xmass[1].push_back(1.0); // Only gas component in gas

  fsp.mass_fraction = xmass;

  // Mobility of each phase
  std::vector<Real> mobilities(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    mobilities[i] = fsp.relperm[i] * fsp.density[i]  / fsp.viscosity[i];

  fsp.mobility = mobilities;

  // For derivatives wrt saturation, the sign of the derivative depends on whether the primary
  // saturation variable is liquid or not. This can be accounted for by multiplying all derivatives
  // wrt S by dS/dSl
  Real sgn = dSaturation_dSl(_svar);

  // Derivative of relative permeability wrt liquid_saturation
  std::vector<Real> drelperm(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    drelperm[i] = sgn * dRelativePermeability(fsp.saturation[0])[i];

  fsp.drelperm = drelperm;

  // Derivative of density wrt pressure
  std::vector<Real> ddensities_dp(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    ddensities_dp[i] = dDensity_dP(fsp.pressure[i], node_temperature, i);

  fsp.ddensity_dp = ddensities_dp;

  // Derivative of density wrt saturation
  std::vector<Real> ddensities_ds(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    ddensities_ds[i] = sgn * dCapillaryPressure(fsp.saturation[0])[i] * fsp.ddensity_dp[i];

  fsp.ddensity_ds = ddensities_ds;

  // Derivative of density wrt mass fraction
  std::vector<std::vector<Real> > ddensities_dx;
  ddensities_dx.resize(numComponents());

  ddensities_dx[0].push_back(0.);
  ddensities_dx[0].push_back(0.);
  ddensities_dx[1].push_back(0.);
  ddensities_dx[1].push_back(0.);

  fsp.ddensity_dx = ddensities_dx;

  // Derivative of mobility wrt pressure
  // Note: dViscosity_dP not implemnted yet
  std::vector<Real> dmobilities_dp(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    dmobilities_dp[i] = fsp.relperm[i] * fsp.ddensity_dp[i]  / fsp.viscosity[i];

  fsp.dmobility_dp = dmobilities_dp;

  // Derivative of mobility wrt saturation
  // Note: dViscosity_dS not implemnted yet
  // Note: drelperm and ddensity_ds are already the correct sign, so don't multiply by sgn
  std::vector<Real> dmobilities_ds(_num_phases);

  for (unsigned int i = 0; i < _num_phases; ++i)
    dmobilities_ds[i] = (fsp.drelperm[i] * fsp.density[i] + fsp.relperm[i] *
      fsp.ddensity_ds[i]) / fsp.viscosity[i];

  fsp.dmobility_ds = dmobilities_ds;

  // Derivative of mobility wrt saturation
  // Note: dViscosity_dX not implemnted yet
  // Note: ddensity_dx is already the correct sign, so don't multiply by sgn
  std::vector<std::vector<Real> > dmobilities_dx(_num_phases);

  for (unsigned int i = 0; i < _num_components; ++i)
    for (unsigned int n = 0; n < _num_phases; ++n)
    dmobilities_dx[i].push_back(fsp.relperm[n] * fsp.ddensity_dx[i][n] / fsp.viscosity[n]);

  fsp.dmobility_dx = dmobilities_dx;
}

unsigned int
FluidStateTwoPhase::getPrimarySaturationVar() const
{
  return _svar;
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
    mooseError("phase_index is out of range in FluidStateTwoPhase::viscosity");

  // TODO: effect of co2 in water on viscosity

  return fluid_viscosity;
}

std::vector<std::vector<Real> >
FluidStateTwoPhase::massFractions(Real pressure, Real temperature) const
{
  std::vector<std::vector<Real> > xmass;
  unsigned int numcomp = numComponents();
//  unsigned int numphase = numPhases();

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

  relperm.push_back(_relative_permeability.relativePermLiquid(liquid_saturation));
  relperm.push_back(_relative_permeability.relativePermGas(liquid_saturation));

  return relperm;
}

std::vector<Real>
FluidStateTwoPhase::dRelativePermeability(Real liquid_saturation) const
{
  std::vector<Real> drelperm;

  drelperm.push_back(_relative_permeability.dRelativePermLiquid(liquid_saturation));
  drelperm.push_back(_relative_permeability.dRelativePermLiquid(liquid_saturation));

  return drelperm;
}

std::vector<Real>
FluidStateTwoPhase::pressure(Real pressure, Real liquid_saturation) const
{
  std::vector<Real> pressures;
  Real capillary_pressure = _capillary_pressure.capillaryPressure(liquid_saturation);

  // Primary pressure is liquid: Pg = Pl - Pc
  if (_p_phase == 0)
  {
    pressures.push_back(pressure);
    pressures.push_back(pressure - capillary_pressure);
  }
  // Primary pressure is gas: Pl = Pg + Pc
  else if (_p_phase == 1)
  {
    pressures.push_back(pressure + capillary_pressure);
    pressures.push_back(pressure);
  }

  return pressures;
}

std::vector<Real>
FluidStateTwoPhase::dCapillaryPressure(Real liquid_saturation) const
{
  std::vector<Real> dpc;

  // Primary pressure is liquid: Pg = Pl - Pc
  if (_p_phase == 0)
  {
  dpc.push_back(0.);
  dpc.push_back(- _capillary_pressure.dCapillaryPressure(liquid_saturation));
  }

  // Primary pressure is gas: Pl = Pg + Pc
  else if (_p_phase == 1)
  {
  dpc.push_back(_capillary_pressure.dCapillaryPressure(liquid_saturation));
  dpc.push_back(0.);
  }
  return dpc;
}

std::vector<Real>
FluidStateTwoPhase::saturation(Real saturation) const
{
  std::vector<Real> saturations;

  // Primary saturation is liquid
  if (_s_phase == 0)
  {
    saturations.push_back(saturation);
    saturations.push_back(1.0 - saturation);
  }
  // Else the primary saturation is gas
  else if (_s_phase == 1)
  {
    saturations.push_back(1.0 - saturation);
    saturations.push_back(saturation);
  }

  return saturations;
}

Real
FluidStateTwoPhase::dSaturation_dS(unsigned int phase_index) const
{
  return (phase_index == 0 ? 1.0 : -1.0);
}

Real
FluidStateTwoPhase::dMassFraction_dX(unsigned int var) const
{
  return (isFluidStateVariable(var) ? 1.0 : -1.0);
}

Real
FluidStateTwoPhase::dSaturation_dSl(unsigned int var) const
{
  Real sgn = 1.0;

  // If saturation is not the primary saturation variable
  if (var != _svar || variablePhase(var) != 0)
    sgn *= -1.0;

  return sgn;
}

Real
FluidStateTwoPhase::dDensity_dP(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real dfluid_density = 0;

  if (phase_index == 0) // liquid phase
    dfluid_density = _liquid_property.dDensity_dP(pressure, temperature);
  else if (phase_index == 1) //gas phase
    dfluid_density = _gas_property.dDensity_dP(pressure, temperature);

  return dfluid_density;
}

Real
FluidStateTwoPhase::dDensity_dX(Real pressure, Real temperature, unsigned int phase_index) const
{
  Real dfluid_density = 0.0;

  return dfluid_density;
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
